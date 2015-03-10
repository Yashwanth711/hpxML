
//  Copyright (c) 2007-2012 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_fwd.hpp>
#include <hpx/runtime/applier/applier.hpp>
#include <hpx/runtime/components/component_factory.hpp>
#include <hpx/runtime/components/derived_component_factory.hpp>
#include <hpx/runtime/actions/continuation.hpp>
#include <hpx/runtime/serialization/exception_ptr.hpp>
#include <hpx/lcos/base_lco_with_value.hpp>
#include <hpx/util/ini.hpp>

HPX_REGISTER_BASE_LCO_WITH_VALUE(float, float)
HPX_REGISTER_BASE_LCO_WITH_VALUE(double, double)
HPX_REGISTER_BASE_LCO_WITH_VALUE(boost::int8_t, int8_t)
HPX_REGISTER_BASE_LCO_WITH_VALUE(boost::uint8_t, uint8_t)
