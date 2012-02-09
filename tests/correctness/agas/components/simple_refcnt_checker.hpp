////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2011 Bryce Adelstein-Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////

#if !defined(HPX_8E877149_A22D_4120_8C0A_BC206BBFA3B4)
#define HPX_8E877149_A22D_4120_8C0A_BC206BBFA3B4

#include <hpx/hpx_fwd.hpp>
#include <hpx/lcos/promise.hpp>
#include <hpx/runtime/components/client_base.hpp>
#include <hpx/runtime/threads/thread_helpers.hpp>
#include <hpx/runtime/agas/remote_interface.hpp>

#include <tests/correctness/agas/components/stubs/simple_refcnt_checker.hpp>

namespace hpx { namespace test
{

struct simple_refcnt_checker
  : components::client_base<
        simple_refcnt_checker
      , stubs::simple_refcnt_checker
    >
{
    typedef components::client_base<
        simple_refcnt_checker
      , stubs::simple_refcnt_checker
    > base_type;

  private:
    lcos::promise<void> flag_;
    naming::id_type const locality_;

    using base_type::create;
    using base_type::create_one;

  public:
    typedef server::simple_refcnt_checker server_type;

    /// Create a new component on the target locality.
    explicit simple_refcnt_checker(
        naming::gid_type const& locality
        )
      : locality_(naming::get_locality_from_gid(locality)
                , naming::id_type::unmanaged)
    {
        this->base_type::create_one(locality, flag_.get_gid());
    }

    /// Create a new component on the target locality.
    explicit simple_refcnt_checker(
        naming::id_type const& locality
        )
      : locality_(naming::get_locality_from_id(locality))
    {
        this->base_type::create_one(locality, flag_.get_gid());
    }

    lcos::promise<void> take_reference_async(
        naming::id_type const& gid
        )
    {
        BOOST_ASSERT(gid_);
        return this->base_type::take_reference_async(gid_, gid);
    }

    void take_reference(
        naming::id_type const& gid
        )
    {
        BOOST_ASSERT(gid_);
        return this->base_type::take_reference(gid_, gid);
    }

    bool ready()
    {
        // Flush pending reference counting operations on the target locality. 
        agas::garbage_collect_sync(locality_);

        return flag_.ready();
    }

    template <
        typename Duration
    >
    bool ready(
        Duration const& d
        )
    {
        // Flush pending reference counting operations on the target locality. 
        agas::garbage_collect_sync(locality_);

        // Schedule a wakeup.
        threads::set_thread_state(threads::get_self_id(), d, threads::pending);

        // Suspend this pxthread.
        threads::get_self().yield(threads::suspended);

        return flag_.ready();
    }
};

}}

#endif // HPX_8E877149_A22D_4120_8C0A_BC206BBFA3B4

