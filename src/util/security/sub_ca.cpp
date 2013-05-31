//  Copyright (c) 2013 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_fwd.hpp>
#include <hpx/util/plugin.hpp>
#include <hpx/util/security/sub_ca.hpp>

#include <boost/function.hpp>

namespace hpx { namespace util { namespace security
{
    sub_ca::~sub_ca()
    {
        // Bind the delete_sub_ca symbol dynamically and invoke it.
        typedef void (*function_type)(ca_type*);
        typedef boost::function<void(function_type)> deleter_type;

        hpx::util::plugin::dll module(
            HPX_MAKE_DLL_STRING(std::string("security")));
        std::pair<function_type, deleter_type> p =
            module.get<function_type, deleter_type>("delete_sub_ca");

        (*p.first)(sub_ca_);
    }

    naming::gid_type const& sub_ca::get_gid() const
    {
        BOOST_ASSERT(0 != sub_ca_);
        return sub_ca_->get_base_gid();
    }

    void sub_ca::init(naming::gid_type const& root_ca)
    {
        // Bind the create_sub_ca symbol dynamically and invoke it.
        typedef ca_type* (*function_type)(
            components::security::server::key_pair const&
          , naming::id_type const&);
        typedef boost::function<void(function_type)> deleter_type;

        hpx::util::plugin::dll module(
            HPX_MAKE_DLL_STRING(std::string("security")));
        std::pair<function_type, deleter_type> p =
            module.get<function_type, deleter_type>("create_sub_ca");

        sub_ca_ = (*p.first)(key_pair_,
            naming::id_type(root_ca, naming::id_type::unmanaged));
    }

    components::security::server::signed_type<
        components::security::server::certificate> sub_ca::get_certificate()
    {
        BOOST_ASSERT(0 != sub_ca_);
        return sub_ca_->get_certificate();
    }
}}}

