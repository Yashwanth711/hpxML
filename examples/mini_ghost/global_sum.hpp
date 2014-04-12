//  Copyright (c) 2014 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_EXAMPLES_MINI_GHOST_GLOBAL_SUM_HPP
#define HPX_EXAMPLES_MINI_GHOST_GLOBAL_SUM_HPP

#include <hpx/include/local_lcos.hpp>

namespace mini_ghost {
    template <typename T>
    struct global_sum
    {
        HPX_MOVABLE_BUT_NOT_COPYABLE(global_sum);
        typedef hpx::lcos::local::spinlock mutex_type;

    public:
        global_sum()
          : value_()
          , generation_(0)
        {
        }

        global_sum(global_sum &&other)
          : value_(std::move(other.value_))
          , generation_(other.generation_)
        {
        }
        global_sum& operator=(global_sum &&other)
        {
            this->value_      = std::move(other.value_);
            this->generation_ = other.generation_;
            return *this;
        }
        template<typename Action>
        hpx::future<T> add(Action action, std::vector<hpx::id_type> ids,
            std::size_t which, T val)
        {
            hpx::future<void> f = gate_.get_future(ids.size(), &generation_);
            HPX_ASSERT(value_ == 0);

            for(hpx::id_type const & id : ids)
            {
                hpx::apply(action, id, generation_, which, val);
            }

            return f.then(
                hpx::launch::sync,
                [this](hpx::future<void>) -> T {
                    T v = value_; value_ = T(0); return v;
                }
            );
        }

        void set_data(std::size_t generation, std::size_t which, T value)
        {
            gate_.synchronize(generation, "global_sum::set_data");
            {
                mutex_type::scoped_lock l(mtx_);
                value_ += value;
            }

            gate_.set(which);         // trigger corresponding and-gate input
        }

    private:
        mutable mutex_type mtx_;

        T value_;
        std::size_t generation_;

        hpx::lcos::local::base_and_gate<> gate_;        // synchronization gate
    };
}

#endif
