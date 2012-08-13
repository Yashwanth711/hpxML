//  Copyright (c) 2007-2012 Hartmut Kaiser
//  Copyright (c)      2012 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_PREPROCESSED_RUNTIME_ACTIONS_DEFINE_FUNCTION_OPERATORS_HPP)
#define HPX_PREPROCESSED_RUNTIME_ACTIONS_DEFINE_FUNCTION_OPERATORS_HPP

#if HPX_ACTION_ARGUMENT_LIMIT  <= 5
#include <hpx/runtime/actions/preprocessed/define_function_operators_5.hpp>
#elif HPX_ACTION_ARGUMENT_LIMIT <= 10
#include <hpx/runtime/actions/preprocessed/define_function_operators_10.hpp>
#elif HPX_ACTION_ARGUMENT_LIMIT <= 15
#include <hpx/runtime/actions/preprocessed/define_function_operators_15.hpp>
#elif HPX_ACTION_ARGUMENT_LIMIT <= 20
#include <hpx/runtime/actions/preprocessed/define_function_operators_20.hpp>
/*
#elif HPX_ACTION_ARGUMENT_LIMIT <= 25
#include <hpx/runtime/actions/preprocessed/define_function_operators_25.hpp>
#elif HPX_ACTION_ARGUMENT_LIMIT <= 30
#include <hpx/runtime/actions/preprocessed/define_function_operators_30.hpp>
#elif HPX_ACTION_ARGUMENT_LIMIT <= 35
#include <hpx/runtime/actions/preprocessed/define_function_operators_35.hpp>
#elif HPX_ACTION_ARGUMENT_LIMIT <= 40
#include <hpx/runtime/actions/preprocessed/define_function_operators_40.hpp>
#elif HPX_ACTION_ARGUMENT_LIMIT <= 45
#include <hpx/runtime/actions/preprocessed/define_function_operators_45.hpp>
#elif HPX_ACTION_ARGUMENT_LIMIT <= 50
#include <hpx/runtime/actions/preprocessed/define_function_operators_50.hpp>
*/
#else
#error "HPX_ACTION_ARGUMENT_LIMIT out of bounds for preprocessed headers"
#endif

#endif
