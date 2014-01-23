//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2014 Thomas Heller
//  Copyright (c) 2011 Katelyn Kufahl
//  Copyright (c) 2011 Bryce Lelbach
//
//  Parts of this code were taken from the Boost.Asio library
//  Copyright (c) 2003-2007 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_PARCELSET_POLICIES_TCP_RECEIVER_HPP
#define HPX_PARCELSET_POLICIES_TCP_RECEIVER_HPP

#include <hpx/util/high_resolution_timer.hpp>
#include <hpx/runtime/parcelset/parcelport_connection.hpp>
#include <hpx/runtime/parcelset/parcel_buffer.hpp>
#include <hpx/runtime/parcelset/decode_parcels.hpp>
#include <hpx/performance_counters/parcels/data_point.hpp>
#include <hpx/performance_counters/parcels/gatherer.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/atomic.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/integer/endian.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

#include <sstream>
#include <vector>

namespace hpx { namespace parcelset
{
    template <typename ConnectionHandler>
    class parcelport_impl;
    
    boost::uint64_t get_max_inbound_size(parcelport&);
}}

namespace hpx { namespace parcelset { namespace policies { namespace tcp
{
    class connection_handler;

    class receiver
      : public parcelport_connection<receiver>
    {
        typedef parcel_buffer<std::vector<char>, std::vector<char> > buffer_type;
    public:
        receiver(boost::asio::io_service& io_service,
            parcelport_impl<connection_handler>& parcelport)
          : socket_(io_service)
          , max_inbound_size_(get_max_inbound_size(parcelport))
          , parcelport_(parcelport)
        {}

        ~receiver()
        {
            // gracefully and portably shutdown the socket
            if(socket_.is_open())
            {
                boost::system::error_code ec;
                socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
                socket_.close(ec);    // close the socket to give it back to the OS
            }
        }

        /// Get the socket associated with the parcelport_connection.
        boost::asio::ip::tcp::socket& socket() { return socket_; }

        /// Asynchronously read a data structure from the socket.
        template <typename Handler>
        void async_read(Handler handler)
        {
            in_buffer_.reset(new buffer_type());

            // Store the time of the begin of the read operation
            in_buffer_->data_point_.time_ = timer_.elapsed_nanoseconds();
            in_buffer_->data_point_.serialization_time_ = 0;
            in_buffer_->data_point_.bytes_ = 0;
            in_buffer_->data_point_.num_parcels_ = 0;

            // Issue a read operation to read the parcel priority and size.
            void (receiver::*f)(boost::system::error_code const&,
                    boost::tuple<Handler>)
                = &receiver::handle_read_header<Handler>;

            using boost::asio::buffer;
            std::vector<boost::asio::mutable_buffer> buffers;
            buffers.push_back(buffer(&in_buffer_->priority_, sizeof(in_buffer_->priority_)));
            buffers.push_back(buffer(&in_buffer_->size_, sizeof(in_buffer_->size_)));
            buffers.push_back(buffer(&in_buffer_->data_size_, sizeof(in_buffer_->data_size_)));

            buffers.push_back(buffer(&in_buffer_->num_chunks_, sizeof(in_buffer_->num_chunks_)));

#if defined(__linux) || defined(linux) || defined(__linux__)
            boost::asio::detail::socket_option::boolean<
                IPPROTO_TCP, TCP_QUICKACK> quickack(true);
            socket_.set_option(quickack);
#endif
            boost::asio::async_read(socket_, buffers,
                boost::bind(f, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::make_tuple(handler)));
        }
    private:
        /// Handle a completed read of the message priority and size from the
        /// message header.
        /// The handler is passed using a tuple since boost::bind seems to have
        /// trouble binding a function object created using boost::bind as a
        /// parameter.
        template <typename Handler>
        void handle_read_header(boost::system::error_code const& e,
            boost::tuple<Handler> handler)
        {
            if (e) {
                boost::get<0>(handler)(e);

                // Issue a read operation to read the next parcel.
                async_read(boost::get<0>(handler));
            }
            else {
                // Determine the length of the serialized data.
                boost::uint64_t inbound_size = in_buffer_->size_;

                if (inbound_size > max_inbound_size_)
                {
                    // report this problem back to the handler
                    boost::get<0>(handler)(boost::asio::error::make_error_code(
                        boost::asio::error::operation_not_supported));
                    return;
                }

                in_buffer_->data_point_.bytes_ = static_cast<std::size_t>(inbound_size);

                // receive buffers
                std::vector<boost::asio::mutable_buffer> buffers;

                // determine the size of the chunk buffer
                std::size_t num_zero_copy_chunks =
                    static_cast<std::size_t>(
                        static_cast<boost::uint32_t>(in_buffer_->num_chunks_.first));
                std::size_t num_non_zero_copy_chunks =
                    static_cast<std::size_t>(
                        static_cast<boost::uint32_t>(in_buffer_->num_chunks_.second));

                void (receiver::*f)(boost::system::error_code const&,
                        boost::tuple<Handler>) = 0;

                if (num_zero_copy_chunks != 0) {
                    in_buffer_->transmission_chunks_.resize(static_cast<std::size_t>(
                        num_zero_copy_chunks + num_non_zero_copy_chunks));

                    buffers.push_back(boost::asio::buffer(in_buffer_->transmission_chunks_.data(),
                        in_buffer_->transmission_chunks_.size()*sizeof(buffer_type::transmission_chunk_type)));

                    // add main buffer holding data which was serialized normally
                    in_buffer_->data_.resize(static_cast<std::size_t>(inbound_size));
                    buffers.push_back(boost::asio::buffer(in_buffer_->data_));

                    // Start an asynchronous call to receive the data.
                    f = &receiver::handle_read_chunk_data<Handler>;
                }
                else {
                    // add main buffer holding data which was serialized normally
                    in_buffer_->data_.resize(static_cast<std::size_t>(inbound_size));
                    buffers.push_back(boost::asio::buffer(in_buffer_->data_));

                    // Start an asynchronous call to receive the data.
                    f = &receiver::handle_read_data<Handler>;
                }

#if defined(__linux) || defined(linux) || defined(__linux__)
                boost::asio::detail::socket_option::boolean<
                    IPPROTO_TCP, TCP_QUICKACK> quickack(true);
                socket_.set_option(quickack);
#endif
                boost::asio::async_read(socket_, buffers,
                    boost::bind(f, shared_from_this(),
                        boost::asio::placeholders::error, handler));
            }
        }

        /// Handle a completed read of message data.
        template <typename Handler>
        void handle_read_chunk_data(boost::system::error_code const& e,
            boost::tuple<Handler> handler)
        {
            if (e) {
                boost::get<0>(handler)(e);

                // Issue a read operation to read the next parcel.
                async_read(boost::get<0>(handler));
            }
            else {
                // receive buffers
                std::vector<boost::asio::mutable_buffer> buffers;

                // add appropriately sized chunk buffers for the zero-copy data
                std::size_t num_zero_copy_chunks =
                    static_cast<std::size_t>(
                        static_cast<boost::uint32_t>(in_buffer_->num_chunks_.first));

                in_buffer_->chunks_.resize(num_zero_copy_chunks);
                for (std::size_t i = 0; i != num_zero_copy_chunks; ++i)
                {
                    std::size_t chunk_size = in_buffer_->transmission_chunks_[i].second;
                    in_buffer_->chunks_[i].resize(chunk_size);
                    buffers.push_back(boost::asio::buffer(in_buffer_->chunks_[i].data(), chunk_size));
                }

                // Start an asynchronous call to receive the data.
                void (receiver::*f)(boost::system::error_code const&,
                        boost::tuple<Handler>)
                    = &receiver::handle_read_data<Handler>;

#if defined(__linux) || defined(linux) || defined(__linux__)
                boost::asio::detail::socket_option::boolean<
                    IPPROTO_TCP, TCP_QUICKACK> quickack(true);
                socket_.set_option(quickack);
#endif
                boost::asio::async_read(socket_, buffers,
                    boost::bind(f, shared_from_this(),
                        boost::asio::placeholders::error, handler));
            }
        }

        /// Handle a completed read of message data.
        template <typename Handler>
        void handle_read_data(boost::system::error_code const& e,
            boost::tuple<Handler> handler)
        {
            if (e) {
                boost::get<0>(handler)(e);

                // Issue a read operation to read the next parcel.
                async_read(boost::get<0>(handler));
            }
            else {
                // complete data point and pass it along
                in_buffer_->data_point_.time_ = timer_.elapsed_nanoseconds() -
                    in_buffer_->data_point_.time_;

                // now send acknowledgment byte
                void (receiver::*f)(boost::system::error_code const&,
                        boost::tuple<Handler>)
                    = &receiver::handle_write_ack<Handler>;

                // decode the received parcels.
                decode_parcels(parcelport_, shared_from_this(), in_buffer_);

                ack_ = true;
                boost::asio::async_write(socket_,
                    boost::asio::buffer(&ack_, sizeof(ack_)),
                    boost::bind(f, shared_from_this(),
                        boost::asio::placeholders::error, handler));
            }
        }

        template <typename Handler>
        void handle_write_ack(boost::system::error_code const& e,
            boost::tuple<Handler> handler)
        {
            // Inform caller that data has been received ok.
            boost::get<0>(handler)(e);

            // Issue a read operation to read the next parcel.
            async_read(boost::get<0>(handler));
        }


        /// Socket for the parcelport_connection.
        boost::asio::ip::tcp::socket socket_;

        /// buffer for incoming data
        boost::shared_ptr<buffer_type> in_buffer_;

        boost::integer::ulittle64_t num_zero_copy_chunks_;
        boost::integer::ulittle64_t num_non_zero_copy_chunks_;

        boost::uint64_t max_inbound_size_;

        bool ack_;

        /// The handler used to process the incoming request.
        parcelport_impl<connection_handler>& parcelport_;

        /// Counters and timers for parcels received.
        util::high_resolution_timer timer_;
    };

    // this makes sure we can store our connections in a set
    inline bool operator<(boost::shared_ptr<receiver> const& lhs,
        boost::shared_ptr<receiver> const& rhs)
    {
        return lhs.get() < rhs.get();
    }
}}}}

#endif
