//  Copyright (c) 2007-2012 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_init.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/components.hpp>

#include <boost/detail/lightweight_test.hpp>

using boost::program_options::variables_map;
using boost::program_options::options_description;

///////////////////////////////////////////////////////////////////////////////
bool null_thread_executed = false;

bool null_thread()
{
    BOOST_TEST(!null_thread_executed);
    null_thread_executed = true;
    return true;
}

// Define the boilerplate code necessary for the function 'null_thread'
// to be invoked as an HPX action (by a HPX future)
typedef hpx::actions::plain_result_action0<bool, null_thread> null_action;

HPX_REGISTER_PLAIN_ACTION(null_action);

typedef hpx::lcos::eager_future<null_action> null_future;

///////////////////////////////////////////////////////////////////////////////
int hpx_main(variables_map&)
{
    // create an explicit future
    null_thread_executed = false;
    {
        null_future f(hpx::find_here());
        BOOST_TEST(f.get());
    }
    BOOST_TEST(null_thread_executed);

    // create an implicit future
    null_thread_executed = false;
    {
        BOOST_TEST(hpx::lcos::wait(hpx::lcos::async<null_action>(hpx::find_here())));
    }
    BOOST_TEST(null_thread_executed);

    hpx::finalize();       // Initiate shutdown of the runtime system.
    return boost::report_errors();
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    // Configure application-specific options.
    options_description
        desc_commandline("usage: " HPX_APPLICATION_STRING " [options]");

    // Initialize and run HPX.
    return hpx::init(desc_commandline, argc, argv);
}

