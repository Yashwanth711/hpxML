//  Copyright (c) 2007-2012 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PP_IS_ITERATING

#if !defined(HPX_LCOS_EAGER_FUTURE_CONSTRUCTORS_JUN_27_2008_0440PM)
#define HPX_LCOS_EAGER_FUTURE_CONSTRUCTORS_JUN_27_2008_0440PM

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/preprocessor/iterate.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>

#define HPX_FWD_ARGS(z, n, _)                                                 \
        BOOST_PP_COMMA_IF(n)                                                  \
            BOOST_FWD_REF(BOOST_PP_CAT(Arg, n)) BOOST_PP_CAT(arg, n)          \
    /**/
#define HPX_FORWARD_ARGS(z, n, _)                                             \
        BOOST_PP_COMMA_IF(n)                                                  \
            boost::forward<BOOST_PP_CAT(Arg, n)>(BOOST_PP_CAT(arg, n))        \
    /**/

#define BOOST_PP_ITERATION_PARAMS_1                                           \
    (3, (2, HPX_ACTION_ARGUMENT_LIMIT,                                        \
    "hpx/lcos/eager_future_constructors.hpp"))                                \
    /**/

#include BOOST_PP_ITERATE()

#undef HPX_FWD_ARGS
#undef HPX_FORWARD_ARGS

#endif

///////////////////////////////////////////////////////////////////////////////
//  Preprocessor vertical repetition code
///////////////////////////////////////////////////////////////////////////////
#else // defined(BOOST_PP_IS_ITERATING)

#define N BOOST_PP_ITERATION()

    template <BOOST_PP_ENUM_PARAMS(N, typename Arg)>
    void apply(naming::id_type const& gid,
        BOOST_PP_REPEAT(N, HPX_FWD_ARGS, _))
    {
        util::block_profiler_wrapper<profiler_tag> bp(apply_logger_);
        hpx::applier::apply_c<Action>(
            this->get_gid(), gid, BOOST_PP_REPEAT(N, HPX_FORWARD_ARGS, _));
    }

    template <BOOST_PP_ENUM_PARAMS(N, typename Arg)>
    void apply_p(naming::id_type const& gid,
        threads::thread_priority priority,
        BOOST_PP_REPEAT(N, HPX_FWD_ARGS, _))
    {
        util::block_profiler_wrapper<profiler_tag> bp(apply_logger_);
        hpx::applier::apply_c_p<Action>(
            this->get_gid(), gid, priority, BOOST_PP_REPEAT(N, HPX_FORWARD_ARGS, _));
    }

    template <BOOST_PP_ENUM_PARAMS(N, typename Arg)>
    eager_future(naming::gid_type const& gid,
            BOOST_PP_REPEAT(N, HPX_FWD_ARGS, _))
      : apply_logger_("eager_future::apply")
    {
        LLCO_(info) << "eager_future::eager_future("
                    << hpx::actions::detail::get_action_name<Action>()
                    << ", "
                    << gid
                    << ") args(" << (N + 1) << ")";
        apply(naming::id_type(gid, naming::id_type::unmanaged),
            BOOST_PP_REPEAT(N, HPX_FORWARD_ARGS, _));
    }
    template <BOOST_PP_ENUM_PARAMS(N, typename Arg)>
    eager_future(naming::id_type const& gid,
            BOOST_PP_REPEAT(N, HPX_FWD_ARGS, _))
      : apply_logger_("eager_future::apply")
    {
        LLCO_(info) << "eager_future::eager_future("
                    << hpx::actions::detail::get_action_name<Action>()
                    << ", "
                    << gid
                    << ") args(" << (N + 1) << ")";
        apply(gid, BOOST_PP_REPEAT(N, HPX_FORWARD_ARGS, _));
    }

    template <BOOST_PP_ENUM_PARAMS(N, typename Arg)>
    eager_future(naming::gid_type const& gid,
            threads::thread_priority priority,
            BOOST_PP_REPEAT(N, HPX_FWD_ARGS, _))
      : apply_logger_("eager_future::apply")
    {
        LLCO_(info) << "eager_future::eager_future("
                    << hpx::actions::detail::get_action_name<Action>()
                    << ", "
                    << gid
                    << ") args(" << (N + 1) << ")";
        apply_p(naming::id_type(gid, naming::id_type::unmanaged),
            priority, BOOST_PP_REPEAT(N, HPX_FORWARD_ARGS, _));
    }
    template <BOOST_PP_ENUM_PARAMS(N, typename Arg)>
    eager_future(naming::id_type const& gid,
            threads::thread_priority priority,
            BOOST_PP_REPEAT(N, HPX_FWD_ARGS, _))
      : apply_logger_("eager_future::apply")
    {
        LLCO_(info) << "eager_future::eager_future("
                    << hpx::actions::detail::get_action_name<Action>()
                    << ", "
                    << gid
                    << ") args(" << (N + 1) << ")";
        apply_p(gid, priority, BOOST_PP_REPEAT(N, HPX_FORWARD_ARGS, _));
    }

#undef N

#endif
