
//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2014-2015 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define HPX_NO_VERSION_CHECK

#include <hpx/hpx_fwd.hpp>

#include <hpx/plugins/parcelport/mpi/mpi_environment.hpp>

#include <hpx/plugins/parcelport_factory.hpp>

#include <hpx/util/command_line_handling.hpp>

// parcelport
#include <hpx/runtime.hpp>
#include <hpx/runtime/parcelset/parcelport.hpp>
#include <hpx/runtime/parcelset/parcel_buffer.hpp>
#include <hpx/runtime/parcelset/encode_parcels.hpp>
#include <hpx/runtime/parcelset/decode_parcels.hpp>
#include <hpx/plugins/parcelport/mpi_new/locality.hpp>
#include <hpx/plugins/parcelport/mpi_new/header.hpp>
#include <hpx/plugins/parcelport/mpi_new/sender.hpp>
#include <hpx/plugins/parcelport/mpi_new/receiver.hpp>

#include <hpx/util/memory_chunk_pool.hpp>

namespace hpx
{
    bool is_starting();
}

namespace hpx { namespace parcelset { namespace policies { namespace mpi
{
    class parcelport
      : public parcelset::parcelport
    {
    private:
        static parcelset::locality here()
        {
            return
                parcelset::locality(
                    mpi_new::locality(
                        util::mpi_environment::rank()
                    )
                );
        }
    public:
        parcelport(util::runtime_configuration const& ini,
            HPX_STD_FUNCTION<void(std::size_t, char const*)> const& on_start_thread,
            HPX_STD_FUNCTION<void()> const& on_stop_thread)
          : parcelset::parcelport(ini, here(), "mpi_new")
          , archive_flags_(boost::archive::no_header)
          , stopped_(false)
          , bootstrapping_(true)
          , chunk_pool_(4194304, 512)
          , sender_(stopped_)
          , receiver_(*this, chunk_pool_, stopped_)
        {
#ifdef BOOST_BIG_ENDIAN
            std::string endian_out = get_config_entry("hpx.parcel.endian_out", "big");
#else
            std::string endian_out = get_config_entry("hpx.parcel.endian_out", "little");
#endif
            if (endian_out == "little")
                archive_flags_ |= util::endian_little;
            else if (endian_out == "big")
                archive_flags_ |= util::endian_big;
            else {
                HPX_ASSERT(endian_out =="little" || endian_out == "big");
            }

            if (!this->allow_array_optimizations()) {
                archive_flags_ |= util::disable_array_optimization;
                archive_flags_ |= util::disable_data_chunking;
            }
            else {
                if (!this->allow_zero_copy_optimizations())
                    archive_flags_ |= util::disable_data_chunking;
            }
        }

        ~parcelport()
        {
            receive_early_parcels_thread_.join();
            util::mpi_environment::finalize();
        }

        bool can_bootstrap() const
        {
            return true;
        }

        parcelset::locality agas_locality(util::runtime_configuration const & ini) const
        {
            return
                parcelset::locality(
                    mpi_new::locality(
                        util::mpi_environment::enabled() ? 0 : -1
                    )
                );
        }

        parcelset::locality create_locality() const
        {
            return parcelset::locality(mpi_new::locality());
        }

        void put_parcels(std::vector<parcelset::locality> dests, std::vector<parcel> parcels,
            std::vector<write_handler_type> handlers)
        {
            HPX_ASSERT(dests.size() == parcels.size());
            HPX_ASSERT(dests.size() == handlers.size());
            for(std::size_t i = 0; i != dests.size(); ++i)
            {
                put_parcel(dests[i], parcels[i], handlers[i]);
            }
        }

        void send_early_parcel(parcelset::locality const & dest, parcel& p)
        {
            put_parcel(dest, p
              , boost::bind(
                    &parcelport::early_write_handler
                  , this
                  , ::_1
                  , p
                )
            );
        }

        void remove_from_connection_cache(parcelset::locality const& loc)
        {}

        util::io_service_pool* get_thread_pool(char const* name)
        {
            return 0;
        }

        boost::int64_t get_connection_cache_statistics(
            connection_cache_statistics_type, bool reset)
        {
            return 0;
        }

        bool run(bool blocking = true)
        {
            receiver_.run();
            receive_early_parcels_thread_ = boost::thread(&parcelport::receive_early_parcels, this, hpx::get_runtime_ptr());
            return true;
        }

        void stop(bool blocking = true)
        {
            stopped_ = true;
            std::cout << "Stop called ...\n";
            if(blocking)
            {
                MPI_Barrier(util::mpi_environment::communicator());
            }
        }

        void enable(bool new_state)
        {
        }

        void put_parcel(parcelset::locality const & dest, parcel p, write_handler_type f)
        {
            //std::cout << "put parcel ...\n";
            if(stopped_) return;

            allocator_type alloc(chunk_pool_);
            snd_buffer_type buffer(alloc);
            encode_parcel(p, buffer, archive_flags_, this->get_max_outbound_message_size());

            int dest_rank = dest.get<mpi_new::locality>().rank();
            HPX_ASSERT(dest_rank != util::mpi_environment::rank());

            sender_.send(dest_rank, buffer, hpx::util::bind(&mpi_new::receiver::receive, boost::ref(receiver_)));

            error_code ec;
            f(ec, p);
            //do_background_work();
        }

        void do_background_work()
        {
            if(stopped_) return;
            receiver_.receive();
        }

    private:
        typedef util::memory_chunk_pool<> memory_pool_type;
        typedef memory_pool_type::allocator allocator_type;
        typedef
            std::vector<char, allocator_type>
            data_type;
        typedef parcel_buffer<data_type> snd_buffer_type;
        typedef parcel_buffer<data_type, data_type> rcv_buffer_type;

        int archive_flags_;

        boost::atomic<bool> stopped_;
        boost::atomic<bool> bootstrapping_;

        memory_pool_type chunk_pool_;

        mpi_new::sender sender_;
        mpi_new::receiver receiver_;

        boost::thread receive_early_parcels_thread_;

        void receive_early_parcels(hpx::runtime * rt)
        {
            rt->register_thread("receive_early_parcel");
            try
            {
                while(rt->get_state() <= runtime::state_startup)
                {
                    do_background_work();
                }
            }
            catch(...)
            {
                rt->unregister_thread();
                throw;
                return;
            }
            rt->unregister_thread();
            std::cout << "finished early parcels ...\n";
        }

        void early_write_handler(
            boost::system::error_code const& ec, parcel const & p)
        {
            if (ec) {
                // all errors during early parcel handling are fatal
                try {
                    HPX_THROW_EXCEPTION(network_error, "early_write_handler",
                        "error while handling early parcel: " +
                            ec.message() + "(" +
                            boost::lexical_cast<std::string>(ec.value())+ ")");
                }
                catch (hpx::exception const& e) {
                    hpx::detail::report_exception_and_terminate(e);
                }
                return;
            }
        }
    };
}}}}

namespace hpx { namespace traits
{
    // Inject additional configuration data into the factory registry for this
    // type. This information ends up in the system wide configuration database
    // under the plugin specific section:
    //
    //      [hpx.parcel.mpi]
    //      ...
    //      priority = 100
    //
    template <>
    struct plugin_config_data<hpx::parcelset::policies::mpi::parcelport>
    {
        static char const* priority()
        {
            return "100";
        }
        static void init(int *argc, char ***argv, util::command_line_handling &cfg)
        {
            util::mpi_environment::init(argc, argv, cfg, "mpi_new");
        }

        static char const* call()
        {
            return
#if defined(HPX_PARCELPORT_MPI_ENV)
                "env = ${HPX_PARCELPORT_MPI_ENV:" HPX_PARCELPORT_MPI_ENV "}\n"
#else
                "env = ${HPX_PARCELPORT_MPI_ENV:PMI_RANK,OMPI_COMM_WORLD_SIZE}\n"
#endif
                "multithreaded = ${HPX_PARCELPORT_MPI_MULTITHREADED:0}\n"
                "io_pool_size = 1\n"
                "use_io_pool = 1\n"
                ;
        }
    };
}}

HPX_REGISTER_PARCELPORT(
    hpx::parcelset::policies::mpi::parcelport,
    mpi_new);
