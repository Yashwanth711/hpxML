//  Copyright (c) 2014 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This is the fifth in a series of examples demonstrating the development of a
// fully distributed solver for a simple 1D heat distribution problem.
//
// This example builds on example four. It adds the possibility to distribute
// both - the locality of the partitions and the locality of where the
// heat_part code is executed. The overall code however still runs on one
// locality only (it is always using hpx::find_here() as the target locality).
//
// This example adds all the boilerplate needed for enabling distributed
// operation. Instead of calling (local) functions we invoke the corresponding
// actions.

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>

#include <boost/shared_array.hpp>

///////////////////////////////////////////////////////////////////////////////
double k = 0.5;     // heat transfer coefficient
double dt = 1.;     // time step
double dx = 1.;     // grid spacing

///////////////////////////////////////////////////////////////////////////////
struct subgrid_data
{
    subgrid_data()
      : size_(0)
    {}

    subgrid_data(std::size_t size)
      : data_(new double [size]), size_(size)
    {}

    subgrid_data(std::size_t size, double initial_value)
      : data_(new double [size]), size_(size)
    {
        double base_value = double(initial_value * size);
        for (std::size_t i = 0; i != size; ++i)
            data_[i] = base_value + double(i);
    }

    double& operator[](std::size_t idx) { return data_[idx]; }
    double operator[](std::size_t idx) const { return data_[idx]; }

    std::size_t size() const { return size_; }

private:
    // Serialization support: even if all of the code below runs on one
    // locality only, we need to provide an (empty) implementation for the
    // serialization as all arguments passed to actions have to support this.
    friend class boost::serialization::access;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version) const {}

private:
    boost::shared_array<double> data_;
    std::size_t size_;
};

std::ostream& operator<<(std::ostream& os, subgrid_data const& c)
{
    os << "{";
    for (std::size_t i = 0; i != c.size(); ++i)
    {
        if (i != 0)
            os << ", ";
        os << c[i];
    }
    os << "}";
    return os;
}

///////////////////////////////////////////////////////////////////////////////
// This is the server side representation of the data. We expose this as a HPX
// component which allows for it to be created and accessed remotely through
// a global address (hpx::id_type).
struct subgrid_server : hpx::components::simple_component_base<subgrid_server>
{
    // construct new instances
    subgrid_server() {}

    subgrid_server(subgrid_data const& data)
      : data_(data)
    {}

    subgrid_server(std::size_t size, double initial_value)
      : data_(size, initial_value)
    {}

    // access data
    subgrid_data get_data() const
    {
        return data_;
    }

    // Every member function which has to be invoked remotely needs to be
    // wrapped into a component action. The macro below defines a new type
    // 'get_data_action' which represents the (possibly remote) member function
    // subgrid::get_data().
    HPX_DEFINE_COMPONENT_CONST_DIRECT_ACTION(subgrid_server, get_data, get_data_action);

private:
    subgrid_data data_;
};

// The macros below are necessary to generate the code required for exposing
// our subgrid type remotely.
//
// HPX_REGISTER_MINIMAL_COMPONENT_FACTORY() exposes the component creation
// through hpx::new_<>().
typedef hpx::components::simple_component<subgrid_server> server_type;
HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(server_type, subgrid_server);

// HPX_REGISTER_ACTION() exposes the component member function for remote
// invocation.
typedef subgrid_server::get_data_action get_data_action;
HPX_REGISTER_ACTION(get_data_action);

///////////////////////////////////////////////////////////////////////////////
// This is a client side helper class allowing to hide some of the tedious
// boilerplate.
struct subgrid : hpx::components::client_base<subgrid, subgrid_server>
{
    typedef hpx::components::client_base<subgrid, subgrid_server> base_type;

    subgrid() {}

    // Create new component on locality 'where' and initialize the held data
    subgrid(hpx::id_type where, std::size_t size, double initial_value)
      : base_type(hpx::new_<subgrid_server>(where, size, initial_value))
    {}

    // Create a new component on the locality co-located to the id 'where'. The
    // new instance will be initialized from the given subgrid_data.
    subgrid(hpx::id_type where, subgrid_data && data)
      : base_type(hpx::new_colocated<subgrid_server>(where, std::move(data)))
    {}

    // Attach a future representing a (possibly remote) subgrid.
    subgrid(hpx::future<hpx::id_type> && id)
      : base_type(std::move(id))
    {}

    // Unwrap a future<subgrid> (a subgrid already holds a future to the id of the
    // referenced object, thus unwrapping accesses this inner future).
    subgrid(hpx::future<subgrid> && c)
      : base_type(std::move(c))
    {}

    ///////////////////////////////////////////////////////////////////////////
    // Invoke the (remote) member function which gives us access to the data.
    // This is a pure helper function hiding the async.
    hpx::future<subgrid_data> get_data() const
    {
        subgrid_server::get_data_action act;
        return hpx::async(act, get_gid());
    }
};

///////////////////////////////////////////////////////////////////////////////
typedef std::vector<subgrid> space;            // data for one time step
typedef std::vector<space> spacetime;       // all of stored time steps

///////////////////////////////////////////////////////////////////////////////
inline std::size_t idx(std::size_t i, std::size_t size)
{
    return (boost::int64_t(i) < 0) ? (i + size) % size : i % size;
}

///////////////////////////////////////////////////////////////////////////////
// Our operator:
inline double heat(double left, double middle, double right)
{
    return middle + (k*dt/dx*dx) * (left - 2*middle + right);
}

// The partitioned operator, it invokes the heat operator above on all elements
// of a partition.
subgrid_data heat_part_data(
    subgrid_data const& left, subgrid_data const& middle, subgrid_data const& right)
{
    std::size_t size = middle.size();
    subgrid_data next(size);

    next[0] = heat(left[size-1], middle[0], middle[1]);

    for (std::size_t i = 1; i != size-1; ++i)
        next[i] = heat(middle[i-1], middle[i], middle[i+1]);

    next[size-1] = heat(middle[size-2], middle[size-1], right[0]);

    return next;
}

///////////////////////////////////////////////////////////////////////////////
subgrid heat_part(subgrid const& left, subgrid const& middle, subgrid const& right)
{
    using hpx::lcos::local::dataflow;
    using hpx::util::unwrapped;

    return dataflow(unwrapped(
        [middle](subgrid_data const& l, subgrid_data const& m, subgrid_data const& r)
        {
            // the new subgrid_data will be allocated on the same locality as 'middle'
            return subgrid(middle.get_gid(), heat_part_data(l, m, r));
        }),
        left.get_data(), middle.get_data(), right.get_data());
}

// Global functions can be exposed as actions as well. That allows to invoke
// those remotely. The macro HPX_PLAIN_ACTION() defines a new action type
// 'heat_part_action' which wraps the global function heat_part(). It can be
// used to call that function on a given locality.
HPX_PLAIN_ACTION(heat_part, heat_part_action);

///////////////////////////////////////////////////////////////////////////////
int hpx_main(boost::program_options::variables_map& vm)
{
    using hpx::lcos::local::dataflow;
    using hpx::util::unwrapped;

    boost::uint64_t nt = vm["nt"].as<boost::uint64_t>();   // Number of steps.
    boost::uint64_t nx = vm["nx"].as<boost::uint64_t>();   // Number of grid points.
    boost::uint64_t np = vm["np"].as<boost::uint64_t>();   // Number of partitions.

    // U[t][i] is the state of position i at time t.
    spacetime U(2);
    for (space& s: U)
        s.resize(np);

    // Initial conditions:
    //   f(0, i) = i
    for (std::size_t i = 0; i != np; ++i)
        U[0][i] = subgrid(hpx::find_here(), nx, double(i));

    using hpx::util::placeholders::_1;
    using hpx::util::placeholders::_2;
    using hpx::util::placeholders::_3;
    auto Op = hpx::util::bind(heat_part_action(), hpx::find_here(), _1, _2, _3);

    for (std::size_t t = 0; t != nt; ++t)
    {
        space& current = U[t % 2];
        space& next = U[(t + 1) % 2];

        for (std::size_t i = 0; i != np; ++i)
            next[i] = dataflow(Op, current[idx(i-1, np)], current[i], current[idx(i+1, np)]);
    }

    // Print the solution at time-step 'nt'.
    space const& solution = U[nt % 2];
    for (std::size_t i = 0; i != np; ++i)
        std::cout << "U[" << i << "] = " << solution[i].get_data().get() << std::endl;

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace boost::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
        ("nx", value<boost::uint64_t>()->default_value(10),
         "Local x dimension (of each partition)")
        ("nt", value<boost::uint64_t>()->default_value(45),
         "Number of time steps")
        ("np", value<boost::uint64_t>()->default_value(10),
         "Number of partitions")
        ("k", value<double>(&k)->default_value(0.5),
         "Heat transfer coefficient (default: 0.5)")
        ("dt", value<double>(&dt)->default_value(1.0),
         "Timestep unit (default: 1.0[s])")
        ("dx", value<double>(&dx)->default_value(1.0),
         "Local x dimension")
    ;

    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}
