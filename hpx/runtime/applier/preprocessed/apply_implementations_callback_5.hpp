// Copyright (c) 2007-2013 Hartmut Kaiser
// Copyright (c) 2012-2013 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This file has been automatically generated using the Boost.Wave tool.
// Do not edit manually.


namespace hpx
{
    
    namespace applier { namespace detail
    {
        template <typename Action, typename Callback,
            typename Arg0>
        inline bool
        apply_r_p_cb(naming::address& addr, naming::id_type const& gid,
            threads::thread_priority priority, BOOST_FWD_REF(Callback) cb,
            BOOST_FWD_REF(Arg0) arg0)
        {
            typedef typename hpx::actions::extract_action<Action>::type action_type;
            
            
            parcelset::parcel p (gid.get_gid(), complement_addr<action_type>(addr),
                new hpx::actions::transfer_action<action_type>(
                    priority, boost::forward<Arg0>( arg0 )));
            
            hpx::applier::get_applier().get_parcel_handler()
                .put_parcel(p, boost::forward<Callback>(cb));
            return false; 
        }
        template <typename Action, typename Callback,
            typename Arg0>
        inline bool
        apply_r_cb(naming::address& addr, naming::id_type const& gid,
            BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0)
        {
            return apply_r_p_cb<Action>(addr, gid,
                actions::action_priority<Action>(), boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ));
        }
    }}
    
    template <typename Action, typename Callback,
        typename Arg0>
    inline bool
    apply_p_cb(naming::id_type const& gid, threads::thread_priority priority,
        BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0)
    {
        
        naming::address addr;
        if (agas::is_local_address(gid, addr)) {
            return applier::detail::apply_l_p<Action>(addr, priority,
                boost::forward<Arg0>( arg0 ));
        }
        
        return applier::detail::apply_r_p_cb<Action>(addr, gid, priority,
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ));
    }
    template <typename Action, typename Callback,
        typename Arg0>
    inline bool
    apply_cb(naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0)
    {
        return apply_p_cb<Action>(gid, actions::action_priority<Action>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ));
    }
    template <typename Component, typename Result,
        typename Arguments, typename Derived, typename Callback,
        typename Arg0>
    inline bool
    apply_cb(
        hpx::actions::action<
            Component, Result, Arguments, Derived
        > ,
        naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0)
    {
        return apply_p_cb<Derived>(gid, actions::action_priority<Derived>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ));
    }
    
    namespace applier { namespace detail
    {
        template <typename Action, typename Callback,
            typename Arg0>
        inline bool
        apply_r_p_cb(naming::address& addr, actions::continuation* c,
            naming::id_type const& gid, threads::thread_priority priority,
            BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0)
        {
            typedef typename hpx::actions::extract_action<Action>::type action_type;
            actions::continuation_type cont(c);
            
            
            parcelset::parcel p (gid.get_gid(), complement_addr<action_type>(addr),
                new hpx::actions::transfer_action<action_type>(
                    priority, boost::forward<Arg0>( arg0 )), cont);
            
            hpx::applier::get_applier().get_parcel_handler()
              .put_parcel(p, boost::forward<Callback>(cb));
            return false; 
        }
        template <typename Action, typename Callback,
            typename Arg0>
        inline bool
        apply_r_cb(naming::address& addr, actions::continuation* c,
            naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
            BOOST_FWD_REF(Arg0) arg0)
        {
            return apply_r_p_cb<Action>(addr, c, gid,
                actions::action_priority<Action>(), boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ));
        }
    }}
    
    template <typename Action, typename Callback,
        typename Arg0>
    inline bool
    apply_p_cb(actions::continuation* c, naming::id_type const& gid,
        threads::thread_priority priority, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0)
    {
        
        naming::address addr;
        if (agas::is_local_address(gid, addr)) {
            return applier::detail::apply_l_p<Action>(c, addr, priority,
                boost::forward<Arg0>( arg0 ));
        }
        
        return applier::detail::apply_r_p_cb<Action>(addr, c, gid, priority,
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ));
    }
    template <typename Action, typename Callback,
        typename Arg0>
    inline bool
    apply_cb(actions::continuation* c, naming::id_type const& gid,
        BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0)
    {
        return apply_p_cb<Action>(c, gid, actions::action_priority<Action>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ));
    }
    template <typename Component, typename Result,
        typename Arguments, typename Derived, typename Callback,
        typename Arg0>
    inline bool
    apply_cb(actions::continuation* c,
        hpx::actions::action<
            Component, Result, Arguments, Derived
        > ,
        naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0)
    {
        return apply_p<Derived>(c, gid, actions::action_priority<Derived>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ));
    }
    
    namespace applier { namespace detail
    {
        template <typename Action, typename Callback,
            typename Arg0>
        inline bool
        apply_c_p_cb(naming::address& addr, naming::id_type const& contgid,
            naming::id_type const& gid, threads::thread_priority priority,
            BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0)
        {
            typedef
                typename hpx::actions::extract_action<Action>::result_type
                result_type;
            return apply_r_p_cb<Action>(addr,
                new actions::typed_continuation<result_type>(contgid),
                gid, priority, boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ));
        }
        template <typename Action, typename Callback,
            typename Arg0>
        inline bool
        apply_c_cb(naming::address& addr, naming::id_type const& contgid,
            naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
            BOOST_FWD_REF(Arg0) arg0)
        {
            typedef
                typename hpx::actions::extract_action<Action>::result_type
                result_type;
            return apply_r_p_cb<Action>(addr,
                new actions::typed_continuation<result_type>(contgid),
                gid, actions::action_priority<Action>(),
                boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ));
        }
    }}
    template <typename Action, typename Callback,
        typename Arg0>
    inline bool
    apply_c_p_cb(naming::id_type const& contgid, naming::id_type const& gid,
        threads::thread_priority priority, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0)
    {
        typedef
            typename hpx::actions::extract_action<Action>::result_type
            result_type;
        return apply_p_cb<Action>(
            new actions::typed_continuation<result_type>(contgid),
            gid, priority, boost::forward<Callback>(cb),
            boost::forward<Arg0>( arg0 ));
    }
    template <typename Action, typename Callback,
        typename Arg0>
    inline bool
    apply_c_cb(naming::id_type const& contgid, naming::id_type const& gid,
        BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0)
    {
        typedef
            typename hpx::actions::extract_action<Action>::result_type
            result_type;
        return apply_p_cb<Action>(
            new actions::typed_continuation<result_type>(contgid),
            gid, actions::action_priority<Action>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ));
    }
}
namespace hpx
{
    
    namespace applier { namespace detail
    {
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1>
        inline bool
        apply_r_p_cb(naming::address& addr, naming::id_type const& gid,
            threads::thread_priority priority, BOOST_FWD_REF(Callback) cb,
            BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1)
        {
            typedef typename hpx::actions::extract_action<Action>::type action_type;
            
            
            parcelset::parcel p (gid.get_gid(), complement_addr<action_type>(addr),
                new hpx::actions::transfer_action<action_type>(
                    priority, boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 )));
            
            hpx::applier::get_applier().get_parcel_handler()
                .put_parcel(p, boost::forward<Callback>(cb));
            return false; 
        }
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1>
        inline bool
        apply_r_cb(naming::address& addr, naming::id_type const& gid,
            BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1)
        {
            return apply_r_p_cb<Action>(addr, gid,
                actions::action_priority<Action>(), boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ));
        }
    }}
    
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1>
    inline bool
    apply_p_cb(naming::id_type const& gid, threads::thread_priority priority,
        BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1)
    {
        
        naming::address addr;
        if (agas::is_local_address(gid, addr)) {
            return applier::detail::apply_l_p<Action>(addr, priority,
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ));
        }
        
        return applier::detail::apply_r_p_cb<Action>(addr, gid, priority,
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ));
    }
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1>
    inline bool
    apply_cb(naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1)
    {
        return apply_p_cb<Action>(gid, actions::action_priority<Action>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ));
    }
    template <typename Component, typename Result,
        typename Arguments, typename Derived, typename Callback,
        typename Arg0 , typename Arg1>
    inline bool
    apply_cb(
        hpx::actions::action<
            Component, Result, Arguments, Derived
        > ,
        naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1)
    {
        return apply_p_cb<Derived>(gid, actions::action_priority<Derived>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ));
    }
    
    namespace applier { namespace detail
    {
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1>
        inline bool
        apply_r_p_cb(naming::address& addr, actions::continuation* c,
            naming::id_type const& gid, threads::thread_priority priority,
            BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1)
        {
            typedef typename hpx::actions::extract_action<Action>::type action_type;
            actions::continuation_type cont(c);
            
            
            parcelset::parcel p (gid.get_gid(), complement_addr<action_type>(addr),
                new hpx::actions::transfer_action<action_type>(
                    priority, boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 )), cont);
            
            hpx::applier::get_applier().get_parcel_handler()
              .put_parcel(p, boost::forward<Callback>(cb));
            return false; 
        }
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1>
        inline bool
        apply_r_cb(naming::address& addr, actions::continuation* c,
            naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
            BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1)
        {
            return apply_r_p_cb<Action>(addr, c, gid,
                actions::action_priority<Action>(), boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ));
        }
    }}
    
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1>
    inline bool
    apply_p_cb(actions::continuation* c, naming::id_type const& gid,
        threads::thread_priority priority, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1)
    {
        
        naming::address addr;
        if (agas::is_local_address(gid, addr)) {
            return applier::detail::apply_l_p<Action>(c, addr, priority,
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ));
        }
        
        return applier::detail::apply_r_p_cb<Action>(addr, c, gid, priority,
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ));
    }
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1>
    inline bool
    apply_cb(actions::continuation* c, naming::id_type const& gid,
        BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1)
    {
        return apply_p_cb<Action>(c, gid, actions::action_priority<Action>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ));
    }
    template <typename Component, typename Result,
        typename Arguments, typename Derived, typename Callback,
        typename Arg0 , typename Arg1>
    inline bool
    apply_cb(actions::continuation* c,
        hpx::actions::action<
            Component, Result, Arguments, Derived
        > ,
        naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1)
    {
        return apply_p<Derived>(c, gid, actions::action_priority<Derived>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ));
    }
    
    namespace applier { namespace detail
    {
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1>
        inline bool
        apply_c_p_cb(naming::address& addr, naming::id_type const& contgid,
            naming::id_type const& gid, threads::thread_priority priority,
            BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1)
        {
            typedef
                typename hpx::actions::extract_action<Action>::result_type
                result_type;
            return apply_r_p_cb<Action>(addr,
                new actions::typed_continuation<result_type>(contgid),
                gid, priority, boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ));
        }
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1>
        inline bool
        apply_c_cb(naming::address& addr, naming::id_type const& contgid,
            naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
            BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1)
        {
            typedef
                typename hpx::actions::extract_action<Action>::result_type
                result_type;
            return apply_r_p_cb<Action>(addr,
                new actions::typed_continuation<result_type>(contgid),
                gid, actions::action_priority<Action>(),
                boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ));
        }
    }}
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1>
    inline bool
    apply_c_p_cb(naming::id_type const& contgid, naming::id_type const& gid,
        threads::thread_priority priority, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1)
    {
        typedef
            typename hpx::actions::extract_action<Action>::result_type
            result_type;
        return apply_p_cb<Action>(
            new actions::typed_continuation<result_type>(contgid),
            gid, priority, boost::forward<Callback>(cb),
            boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ));
    }
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1>
    inline bool
    apply_c_cb(naming::id_type const& contgid, naming::id_type const& gid,
        BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1)
    {
        typedef
            typename hpx::actions::extract_action<Action>::result_type
            result_type;
        return apply_p_cb<Action>(
            new actions::typed_continuation<result_type>(contgid),
            gid, actions::action_priority<Action>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ));
    }
}
namespace hpx
{
    
    namespace applier { namespace detail
    {
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2>
        inline bool
        apply_r_p_cb(naming::address& addr, naming::id_type const& gid,
            threads::thread_priority priority, BOOST_FWD_REF(Callback) cb,
            BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2)
        {
            typedef typename hpx::actions::extract_action<Action>::type action_type;
            
            
            parcelset::parcel p (gid.get_gid(), complement_addr<action_type>(addr),
                new hpx::actions::transfer_action<action_type>(
                    priority, boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 )));
            
            hpx::applier::get_applier().get_parcel_handler()
                .put_parcel(p, boost::forward<Callback>(cb));
            return false; 
        }
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2>
        inline bool
        apply_r_cb(naming::address& addr, naming::id_type const& gid,
            BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2)
        {
            return apply_r_p_cb<Action>(addr, gid,
                actions::action_priority<Action>(), boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ));
        }
    }}
    
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2>
    inline bool
    apply_p_cb(naming::id_type const& gid, threads::thread_priority priority,
        BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2)
    {
        
        naming::address addr;
        if (agas::is_local_address(gid, addr)) {
            return applier::detail::apply_l_p<Action>(addr, priority,
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ));
        }
        
        return applier::detail::apply_r_p_cb<Action>(addr, gid, priority,
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ));
    }
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2>
    inline bool
    apply_cb(naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2)
    {
        return apply_p_cb<Action>(gid, actions::action_priority<Action>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ));
    }
    template <typename Component, typename Result,
        typename Arguments, typename Derived, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2>
    inline bool
    apply_cb(
        hpx::actions::action<
            Component, Result, Arguments, Derived
        > ,
        naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2)
    {
        return apply_p_cb<Derived>(gid, actions::action_priority<Derived>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ));
    }
    
    namespace applier { namespace detail
    {
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2>
        inline bool
        apply_r_p_cb(naming::address& addr, actions::continuation* c,
            naming::id_type const& gid, threads::thread_priority priority,
            BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2)
        {
            typedef typename hpx::actions::extract_action<Action>::type action_type;
            actions::continuation_type cont(c);
            
            
            parcelset::parcel p (gid.get_gid(), complement_addr<action_type>(addr),
                new hpx::actions::transfer_action<action_type>(
                    priority, boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 )), cont);
            
            hpx::applier::get_applier().get_parcel_handler()
              .put_parcel(p, boost::forward<Callback>(cb));
            return false; 
        }
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2>
        inline bool
        apply_r_cb(naming::address& addr, actions::continuation* c,
            naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
            BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2)
        {
            return apply_r_p_cb<Action>(addr, c, gid,
                actions::action_priority<Action>(), boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ));
        }
    }}
    
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2>
    inline bool
    apply_p_cb(actions::continuation* c, naming::id_type const& gid,
        threads::thread_priority priority, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2)
    {
        
        naming::address addr;
        if (agas::is_local_address(gid, addr)) {
            return applier::detail::apply_l_p<Action>(c, addr, priority,
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ));
        }
        
        return applier::detail::apply_r_p_cb<Action>(addr, c, gid, priority,
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ));
    }
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2>
    inline bool
    apply_cb(actions::continuation* c, naming::id_type const& gid,
        BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2)
    {
        return apply_p_cb<Action>(c, gid, actions::action_priority<Action>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ));
    }
    template <typename Component, typename Result,
        typename Arguments, typename Derived, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2>
    inline bool
    apply_cb(actions::continuation* c,
        hpx::actions::action<
            Component, Result, Arguments, Derived
        > ,
        naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2)
    {
        return apply_p<Derived>(c, gid, actions::action_priority<Derived>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ));
    }
    
    namespace applier { namespace detail
    {
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2>
        inline bool
        apply_c_p_cb(naming::address& addr, naming::id_type const& contgid,
            naming::id_type const& gid, threads::thread_priority priority,
            BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2)
        {
            typedef
                typename hpx::actions::extract_action<Action>::result_type
                result_type;
            return apply_r_p_cb<Action>(addr,
                new actions::typed_continuation<result_type>(contgid),
                gid, priority, boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ));
        }
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2>
        inline bool
        apply_c_cb(naming::address& addr, naming::id_type const& contgid,
            naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
            BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2)
        {
            typedef
                typename hpx::actions::extract_action<Action>::result_type
                result_type;
            return apply_r_p_cb<Action>(addr,
                new actions::typed_continuation<result_type>(contgid),
                gid, actions::action_priority<Action>(),
                boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ));
        }
    }}
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2>
    inline bool
    apply_c_p_cb(naming::id_type const& contgid, naming::id_type const& gid,
        threads::thread_priority priority, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2)
    {
        typedef
            typename hpx::actions::extract_action<Action>::result_type
            result_type;
        return apply_p_cb<Action>(
            new actions::typed_continuation<result_type>(contgid),
            gid, priority, boost::forward<Callback>(cb),
            boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ));
    }
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2>
    inline bool
    apply_c_cb(naming::id_type const& contgid, naming::id_type const& gid,
        BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2)
    {
        typedef
            typename hpx::actions::extract_action<Action>::result_type
            result_type;
        return apply_p_cb<Action>(
            new actions::typed_continuation<result_type>(contgid),
            gid, actions::action_priority<Action>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ));
    }
}
namespace hpx
{
    
    namespace applier { namespace detail
    {
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3>
        inline bool
        apply_r_p_cb(naming::address& addr, naming::id_type const& gid,
            threads::thread_priority priority, BOOST_FWD_REF(Callback) cb,
            BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3)
        {
            typedef typename hpx::actions::extract_action<Action>::type action_type;
            
            
            parcelset::parcel p (gid.get_gid(), complement_addr<action_type>(addr),
                new hpx::actions::transfer_action<action_type>(
                    priority, boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 )));
            
            hpx::applier::get_applier().get_parcel_handler()
                .put_parcel(p, boost::forward<Callback>(cb));
            return false; 
        }
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3>
        inline bool
        apply_r_cb(naming::address& addr, naming::id_type const& gid,
            BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3)
        {
            return apply_r_p_cb<Action>(addr, gid,
                actions::action_priority<Action>(), boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ));
        }
    }}
    
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3>
    inline bool
    apply_p_cb(naming::id_type const& gid, threads::thread_priority priority,
        BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3)
    {
        
        naming::address addr;
        if (agas::is_local_address(gid, addr)) {
            return applier::detail::apply_l_p<Action>(addr, priority,
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ));
        }
        
        return applier::detail::apply_r_p_cb<Action>(addr, gid, priority,
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ));
    }
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3>
    inline bool
    apply_cb(naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3)
    {
        return apply_p_cb<Action>(gid, actions::action_priority<Action>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ));
    }
    template <typename Component, typename Result,
        typename Arguments, typename Derived, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3>
    inline bool
    apply_cb(
        hpx::actions::action<
            Component, Result, Arguments, Derived
        > ,
        naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3)
    {
        return apply_p_cb<Derived>(gid, actions::action_priority<Derived>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ));
    }
    
    namespace applier { namespace detail
    {
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3>
        inline bool
        apply_r_p_cb(naming::address& addr, actions::continuation* c,
            naming::id_type const& gid, threads::thread_priority priority,
            BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3)
        {
            typedef typename hpx::actions::extract_action<Action>::type action_type;
            actions::continuation_type cont(c);
            
            
            parcelset::parcel p (gid.get_gid(), complement_addr<action_type>(addr),
                new hpx::actions::transfer_action<action_type>(
                    priority, boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 )), cont);
            
            hpx::applier::get_applier().get_parcel_handler()
              .put_parcel(p, boost::forward<Callback>(cb));
            return false; 
        }
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3>
        inline bool
        apply_r_cb(naming::address& addr, actions::continuation* c,
            naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
            BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3)
        {
            return apply_r_p_cb<Action>(addr, c, gid,
                actions::action_priority<Action>(), boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ));
        }
    }}
    
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3>
    inline bool
    apply_p_cb(actions::continuation* c, naming::id_type const& gid,
        threads::thread_priority priority, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3)
    {
        
        naming::address addr;
        if (agas::is_local_address(gid, addr)) {
            return applier::detail::apply_l_p<Action>(c, addr, priority,
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ));
        }
        
        return applier::detail::apply_r_p_cb<Action>(addr, c, gid, priority,
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ));
    }
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3>
    inline bool
    apply_cb(actions::continuation* c, naming::id_type const& gid,
        BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3)
    {
        return apply_p_cb<Action>(c, gid, actions::action_priority<Action>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ));
    }
    template <typename Component, typename Result,
        typename Arguments, typename Derived, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3>
    inline bool
    apply_cb(actions::continuation* c,
        hpx::actions::action<
            Component, Result, Arguments, Derived
        > ,
        naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3)
    {
        return apply_p<Derived>(c, gid, actions::action_priority<Derived>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ));
    }
    
    namespace applier { namespace detail
    {
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3>
        inline bool
        apply_c_p_cb(naming::address& addr, naming::id_type const& contgid,
            naming::id_type const& gid, threads::thread_priority priority,
            BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3)
        {
            typedef
                typename hpx::actions::extract_action<Action>::result_type
                result_type;
            return apply_r_p_cb<Action>(addr,
                new actions::typed_continuation<result_type>(contgid),
                gid, priority, boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ));
        }
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3>
        inline bool
        apply_c_cb(naming::address& addr, naming::id_type const& contgid,
            naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
            BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3)
        {
            typedef
                typename hpx::actions::extract_action<Action>::result_type
                result_type;
            return apply_r_p_cb<Action>(addr,
                new actions::typed_continuation<result_type>(contgid),
                gid, actions::action_priority<Action>(),
                boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ));
        }
    }}
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3>
    inline bool
    apply_c_p_cb(naming::id_type const& contgid, naming::id_type const& gid,
        threads::thread_priority priority, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3)
    {
        typedef
            typename hpx::actions::extract_action<Action>::result_type
            result_type;
        return apply_p_cb<Action>(
            new actions::typed_continuation<result_type>(contgid),
            gid, priority, boost::forward<Callback>(cb),
            boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ));
    }
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3>
    inline bool
    apply_c_cb(naming::id_type const& contgid, naming::id_type const& gid,
        BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3)
    {
        typedef
            typename hpx::actions::extract_action<Action>::result_type
            result_type;
        return apply_p_cb<Action>(
            new actions::typed_continuation<result_type>(contgid),
            gid, actions::action_priority<Action>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ));
    }
}
namespace hpx
{
    
    namespace applier { namespace detail
    {
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3 , typename Arg4>
        inline bool
        apply_r_p_cb(naming::address& addr, naming::id_type const& gid,
            threads::thread_priority priority, BOOST_FWD_REF(Callback) cb,
            BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3 , BOOST_FWD_REF(Arg4) arg4)
        {
            typedef typename hpx::actions::extract_action<Action>::type action_type;
            
            
            parcelset::parcel p (gid.get_gid(), complement_addr<action_type>(addr),
                new hpx::actions::transfer_action<action_type>(
                    priority, boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ) , boost::forward<Arg4>( arg4 )));
            
            hpx::applier::get_applier().get_parcel_handler()
                .put_parcel(p, boost::forward<Callback>(cb));
            return false; 
        }
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3 , typename Arg4>
        inline bool
        apply_r_cb(naming::address& addr, naming::id_type const& gid,
            BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3 , BOOST_FWD_REF(Arg4) arg4)
        {
            return apply_r_p_cb<Action>(addr, gid,
                actions::action_priority<Action>(), boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ) , boost::forward<Arg4>( arg4 ));
        }
    }}
    
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3 , typename Arg4>
    inline bool
    apply_p_cb(naming::id_type const& gid, threads::thread_priority priority,
        BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3 , BOOST_FWD_REF(Arg4) arg4)
    {
        
        naming::address addr;
        if (agas::is_local_address(gid, addr)) {
            return applier::detail::apply_l_p<Action>(addr, priority,
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ) , boost::forward<Arg4>( arg4 ));
        }
        
        return applier::detail::apply_r_p_cb<Action>(addr, gid, priority,
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ) , boost::forward<Arg4>( arg4 ));
    }
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3 , typename Arg4>
    inline bool
    apply_cb(naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3 , BOOST_FWD_REF(Arg4) arg4)
    {
        return apply_p_cb<Action>(gid, actions::action_priority<Action>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ) , boost::forward<Arg4>( arg4 ));
    }
    template <typename Component, typename Result,
        typename Arguments, typename Derived, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3 , typename Arg4>
    inline bool
    apply_cb(
        hpx::actions::action<
            Component, Result, Arguments, Derived
        > ,
        naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3 , BOOST_FWD_REF(Arg4) arg4)
    {
        return apply_p_cb<Derived>(gid, actions::action_priority<Derived>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ) , boost::forward<Arg4>( arg4 ));
    }
    
    namespace applier { namespace detail
    {
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3 , typename Arg4>
        inline bool
        apply_r_p_cb(naming::address& addr, actions::continuation* c,
            naming::id_type const& gid, threads::thread_priority priority,
            BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3 , BOOST_FWD_REF(Arg4) arg4)
        {
            typedef typename hpx::actions::extract_action<Action>::type action_type;
            actions::continuation_type cont(c);
            
            
            parcelset::parcel p (gid.get_gid(), complement_addr<action_type>(addr),
                new hpx::actions::transfer_action<action_type>(
                    priority, boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ) , boost::forward<Arg4>( arg4 )), cont);
            
            hpx::applier::get_applier().get_parcel_handler()
              .put_parcel(p, boost::forward<Callback>(cb));
            return false; 
        }
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3 , typename Arg4>
        inline bool
        apply_r_cb(naming::address& addr, actions::continuation* c,
            naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
            BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3 , BOOST_FWD_REF(Arg4) arg4)
        {
            return apply_r_p_cb<Action>(addr, c, gid,
                actions::action_priority<Action>(), boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ) , boost::forward<Arg4>( arg4 ));
        }
    }}
    
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3 , typename Arg4>
    inline bool
    apply_p_cb(actions::continuation* c, naming::id_type const& gid,
        threads::thread_priority priority, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3 , BOOST_FWD_REF(Arg4) arg4)
    {
        
        naming::address addr;
        if (agas::is_local_address(gid, addr)) {
            return applier::detail::apply_l_p<Action>(c, addr, priority,
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ) , boost::forward<Arg4>( arg4 ));
        }
        
        return applier::detail::apply_r_p_cb<Action>(addr, c, gid, priority,
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ) , boost::forward<Arg4>( arg4 ));
    }
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3 , typename Arg4>
    inline bool
    apply_cb(actions::continuation* c, naming::id_type const& gid,
        BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3 , BOOST_FWD_REF(Arg4) arg4)
    {
        return apply_p_cb<Action>(c, gid, actions::action_priority<Action>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ) , boost::forward<Arg4>( arg4 ));
    }
    template <typename Component, typename Result,
        typename Arguments, typename Derived, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3 , typename Arg4>
    inline bool
    apply_cb(actions::continuation* c,
        hpx::actions::action<
            Component, Result, Arguments, Derived
        > ,
        naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3 , BOOST_FWD_REF(Arg4) arg4)
    {
        return apply_p<Derived>(c, gid, actions::action_priority<Derived>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ) , boost::forward<Arg4>( arg4 ));
    }
    
    namespace applier { namespace detail
    {
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3 , typename Arg4>
        inline bool
        apply_c_p_cb(naming::address& addr, naming::id_type const& contgid,
            naming::id_type const& gid, threads::thread_priority priority,
            BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3 , BOOST_FWD_REF(Arg4) arg4)
        {
            typedef
                typename hpx::actions::extract_action<Action>::result_type
                result_type;
            return apply_r_p_cb<Action>(addr,
                new actions::typed_continuation<result_type>(contgid),
                gid, priority, boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ) , boost::forward<Arg4>( arg4 ));
        }
        template <typename Action, typename Callback,
            typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3 , typename Arg4>
        inline bool
        apply_c_cb(naming::address& addr, naming::id_type const& contgid,
            naming::id_type const& gid, BOOST_FWD_REF(Callback) cb,
            BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3 , BOOST_FWD_REF(Arg4) arg4)
        {
            typedef
                typename hpx::actions::extract_action<Action>::result_type
                result_type;
            return apply_r_p_cb<Action>(addr,
                new actions::typed_continuation<result_type>(contgid),
                gid, actions::action_priority<Action>(),
                boost::forward<Callback>(cb),
                boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ) , boost::forward<Arg4>( arg4 ));
        }
    }}
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3 , typename Arg4>
    inline bool
    apply_c_p_cb(naming::id_type const& contgid, naming::id_type const& gid,
        threads::thread_priority priority, BOOST_FWD_REF(Callback) cb,
        BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3 , BOOST_FWD_REF(Arg4) arg4)
    {
        typedef
            typename hpx::actions::extract_action<Action>::result_type
            result_type;
        return apply_p_cb<Action>(
            new actions::typed_continuation<result_type>(contgid),
            gid, priority, boost::forward<Callback>(cb),
            boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ) , boost::forward<Arg4>( arg4 ));
    }
    template <typename Action, typename Callback,
        typename Arg0 , typename Arg1 , typename Arg2 , typename Arg3 , typename Arg4>
    inline bool
    apply_c_cb(naming::id_type const& contgid, naming::id_type const& gid,
        BOOST_FWD_REF(Callback) cb, BOOST_FWD_REF(Arg0) arg0 , BOOST_FWD_REF(Arg1) arg1 , BOOST_FWD_REF(Arg2) arg2 , BOOST_FWD_REF(Arg3) arg3 , BOOST_FWD_REF(Arg4) arg4)
    {
        typedef
            typename hpx::actions::extract_action<Action>::result_type
            result_type;
        return apply_p_cb<Action>(
            new actions::typed_continuation<result_type>(contgid),
            gid, actions::action_priority<Action>(),
            boost::forward<Callback>(cb), boost::forward<Arg0>( arg0 ) , boost::forward<Arg1>( arg1 ) , boost::forward<Arg2>( arg2 ) , boost::forward<Arg3>( arg3 ) , boost::forward<Arg4>( arg4 ));
    }
}
