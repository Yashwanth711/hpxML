////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2011 Bryce Lelbach
//  Copyright (c) 2011-2016 Hartmut Kaiser
//  Copyright (c) 2016 Parsa Amini
//  Copyright (c) 2016 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////

#if !defined(HPX_15D904C7_CD18_46E1_A54A_65059966A34F)
#define HPX_15D904C7_CD18_46E1_A54A_65059966A34F

#include <hpx/config.hpp>
#include <hpx/exception_fwd.hpp>
#include <hpx/state.hpp>
#include <hpx/lcos/local/mutex.hpp>
#include <hpx/include/async.hpp>
#include <hpx/runtime/applier/applier.hpp>
#include <hpx/runtime/naming/address.hpp>
#include <hpx/runtime/naming/name.hpp>
#include <hpx/runtime/components/pinned_ptr.hpp>
#include <hpx/runtime/agas/detail/agas_service_client.hpp>
#include <hpx/util/function.hpp>
#include <hpx/util/unique_function.hpp>

#include <boost/make_shared.hpp>
#include <boost/cache/lru_cache.hpp>
#include <boost/cache/statistics/local_full_statistics.hpp>
#include <boost/cstdint.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/thread/locks.hpp>

#include <map>
#include <vector>

namespace hpx { namespace util
{
    class runtime_configuration;
}}

namespace hpx { namespace agas
{
struct request;
struct response;
HPX_EXPORT void destroy_big_boot_barrier();

struct HPX_EXPORT addressing_service
{
private:
    HPX_NON_COPYABLE(addressing_service);

public:
    // {{{ types
    typedef components::component_type component_id_type;

    typedef hpx::util::function<
        void(std::string const&, naming::gid_type const&)
    > iterate_names_function_type;

    typedef hpx::util::function<
        void(std::string const&, components::component_type)
    > iterate_types_function_type;

    typedef hpx::lcos::local::spinlock mutex_type;
    // }}}

    // {{{ gva cache
    struct gva_cache_key;

    typedef boost::cache::lru_cache<
        gva_cache_key
      , gva
      , boost::cache::statistics::local_full_statistics
    > gva_cache_type;
    // }}}

    typedef std::set<naming::gid_type> migrated_objects_table_type;
    typedef std::map<naming::gid_type, boost::int64_t> refcnt_requests_type;

    mutable mutex_type gva_cache_mtx_;
    boost::shared_ptr<gva_cache_type> gva_cache_;

    mutable mutex_type migrated_objects_mtx_;
    migrated_objects_table_type migrated_objects_table_;

    mutable mutex_type console_cache_mtx_;
    boost::uint32_t console_cache_;

    std::size_t const max_refcnt_requests_;

    mutex_type refcnt_requests_mtx_;
    std::size_t refcnt_requests_count_;
    bool enable_refcnt_caching_;

    boost::shared_ptr<refcnt_requests_type> refcnt_requests_;

    service_mode const service_type;
    runtime_mode const runtime_type;

    bool const caching_;
    bool const range_caching_;
    threads::thread_priority const action_priority_;

    boost::uint64_t rts_lva_;
    boost::uint64_t mem_lva_;

    boost::shared_ptr<detail::agas_service_client> client_;

    boost::atomic<hpx::state> state_;
    naming::gid_type locality_;

    naming::address locality_ns_addr_;
    naming::address primary_ns_addr_;
    naming::address component_ns_addr_;
    naming::address symbol_ns_addr_;

    mutable mutex_type resolved_localities_mtx_;
    typedef
        std::map<naming::gid_type, parcelset::endpoints_type>
        resolved_localities_type;
    resolved_localities_type resolved_localities_;

    addressing_service(
        parcelset::parcelhandler& ph
      , util::runtime_configuration const& ini_
      , runtime_mode runtime_type_
        );

    ~addressing_service()
    {
        // TODO: Free the future pools?
        destroy_big_boot_barrier();
    }

    void initialize(parcelset::parcelhandler& ph, boost::uint64_t rts_lva,
        boost::uint64_t mem_lva);

    /// \brief Adjust the size of the local AGAS Address resolution cache
    void adjust_local_cache_size(std::size_t);

    state get_status() const
    {
        if (!client_)
            return state_stopping;
        return state_.load();
    }

    void set_status(state new_state)
    {
        state_.store(new_state);
    }

    naming::gid_type const& get_local_locality(error_code& ec = throws) const
    {
        return locality_;
    }

    void set_local_locality(naming::gid_type const& g);
    void register_console(parcelset::endpoints_type const & eps);

    bool is_bootstrap() const
    {
        return service_type == service_mode_bootstrap;
    }

    /// \brief Returns whether this addressing_service represents the console
    ///        locality.
    bool is_console() const
    {
        return runtime_type == runtime_mode_console;
    }

    /// \brief Returns whether this addressing_service is connecting to a
    ///        running application
    bool is_connecting() const
    {
        return runtime_type == runtime_mode_connect;
    }

    bool resolve_locally_known_addresses(
        naming::gid_type const& id
      , naming::address& addr
      );

    /// \brief Register performance counter types exposing properties from the
    ///        local cache.
    void register_counter_types();

    // FIXME: document (add comments)
    void garbage_collect_non_blocking(
        error_code& ec = throws
        );

    // FIXME: document (add comments)
    void garbage_collect(
        error_code& ec = throws
        );

    naming::address::address_type get_hosted_primary_ns_ptr() const;
    naming::address::address_type get_hosted_symbol_ns_ptr() const;

    naming::address::address_type get_bootstrap_locality_ns_ptr() const;
    naming::address::address_type get_bootstrap_primary_ns_ptr() const;
    naming::address::address_type get_bootstrap_component_ns_ptr() const;
    naming::address::address_type get_bootstrap_symbol_ns_ptr() const;

    boost::int64_t synchronize_with_async_incref(
        hpx::future<boost::int64_t> fut
      , naming::id_type const& id
      , boost::int64_t compensated_credit
        );

    naming::address::address_type get_primary_ns_lva() const
    {
        return client_->get_primary_ns_ptr();
    }

    naming::address::address_type get_symbol_ns_lva() const
    {
        return client_->get_symbol_ns_ptr();
    }

protected:
    void launch_bootstrap(
        boost::shared_ptr<parcelset::parcelport> const& pp
      , parcelset::endpoints_type const & endpoints
      , util::runtime_configuration const& ini_
        );

    void launch_hosted();

    naming::address resolve_full_postproc(
        future<response> f
      , naming::gid_type const& id
        );
    bool bind_postproc(
        future<response> f
      , naming::gid_type const& id
      , gva const& g
        );

    /// Maintain list of migrated objects
    bool was_object_migrated_locked(
        naming::gid_type const& id
        );

private:
    /// Assumes that \a refcnt_requests_mtx_ is locked.
    void send_refcnt_requests(
        boost::unique_lock<mutex_type>& l
      , error_code& ec = throws
        );

    /// Assumes that \a refcnt_requests_mtx_ is locked.
    void send_refcnt_requests_non_blocking(
        boost::unique_lock<mutex_type>& l
      , error_code& ec
        );

    /// Assumes that \a refcnt_requests_mtx_ is locked.
    std::vector<hpx::future<std::vector<response> > >
    send_refcnt_requests_async(
        boost::unique_lock<mutex_type>& l
        );

    /// Assumes that \a refcnt_requests_mtx_ is locked.
    void send_refcnt_requests_sync(
        boost::unique_lock<mutex_type>& l
      , error_code& ec
        );

    // Helper functions to access the current cache statistics
    boost::uint64_t get_cache_entries(bool);
    boost::uint64_t get_cache_hits(bool);
    boost::uint64_t get_cache_misses(bool);
    boost::uint64_t get_cache_evictions(bool);
    boost::uint64_t get_cache_insertions(bool);

    boost::uint64_t get_cache_get_entry_count(bool reset);
    boost::uint64_t get_cache_insertion_entry_count(bool reset);
    boost::uint64_t get_cache_update_entry_count(bool reset);
    boost::uint64_t get_cache_erase_entry_count(bool reset);

    boost::uint64_t get_cache_get_entry_time(bool reset);
    boost::uint64_t get_cache_insertion_entry_time(bool reset);
    boost::uint64_t get_cache_update_entry_time(bool reset);
    boost::uint64_t get_cache_erase_entry_time(bool reset);

public:
    response service(
        request const& req
      , error_code& ec = throws
        );

    std::vector<response> bulk_service(
        std::vector<request> const& reqs
      , error_code& ec = throws
        );

    /// \brief Add a locality to the runtime.
    bool register_locality(
        parcelset::endpoints_type const & endpoints
      , naming::gid_type& prefix
      , boost::uint32_t num_threads
      , error_code& ec = throws
        );

    /// \brief Resolve a locality to its prefix.
    ///
    /// \returns Returns an empty vector if the locality is not registered.
    parcelset::endpoints_type const& resolve_locality(
        naming::gid_type const & gid
      , error_code& ec = throws
        );

    bool has_resolved_locality(
        naming::gid_type const & gid
        );

    /// \brief Remove a locality from the runtime.
    bool unregister_locality(
        naming::gid_type const & gid
      , error_code& ec = throws
        );

    /// \brief remove given locality from locality cache
    void remove_resolved_locality(naming::gid_type const& gid);

    /// \brief Get locality locality_id of the console locality.
    ///
    /// \param locality_id     [out] The locality_id value uniquely identifying the
    ///                   console locality. This is valid only, if the
    ///                   return value of this function is true.
    /// \param try_cache  [in] If this is set to true the console is first
    ///                   tried to be found in the local cache. Otherwise
    ///                   this function will always query AGAS, even if the
    ///                   console locality_id is already known locally.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \returns          This function returns \a true if a console locality_id
    ///                   exists and returns \a false otherwise.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    bool get_console_locality(
        naming::gid_type& locality
      , error_code& ec = throws
        );

    /// \brief Query for the locality_ids of all known localities.
    ///
    /// This function returns the locality_ids of all localities known to the
    /// AGAS server or all localities having a registered factory for a
    /// given component type.
    ///
    /// \param locality_ids [out] The vector will contain the prefixes of all
    ///                   localities registered with the AGAS server. The
    ///                   returned vector holds the prefixes representing
    ///                   the runtime_support components of these
    ///                   localities.
    /// \param type       [in] The component type will be used to determine
    ///                   the set of prefixes having a registered factory
    ///                   for this component. The default value for this
    ///                   parameter is \a components#component_invalid,
    ///                   which will return prefixes of all localities.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    bool get_localities(
        std::vector<naming::gid_type>& locality_ids
      , components::component_type type
      , error_code& ec = throws
        );

    bool get_localities(
        std::vector<naming::gid_type>& locality_ids
      , error_code& ec = throws
        )
    {
        return get_localities(locality_ids, components::component_invalid, ec);
    }

    /// \brief Query the resolved addresses for all know localities
    ///
    /// This function returns the resolved addresses for all localities known
    /// to the AGAS server.
    ///
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    std::map<naming::gid_type, parcelset::endpoints_type>
    get_resolved_localities(error_code& ec = throws);

    /// \brief Query for the number of all known localities.
    ///
    /// This function returns the number of localities known to the AGAS server
    /// or the number of localities having a registered factory for a given
    /// component type.
    ///
    /// \param type       [in] The component type will be used to determine
    ///                   the set of prefixes having a registered factory
    ///                   for this component. The default value for this
    ///                   parameter is \a components#component_invalid,
    ///                   which will return prefixes of all localities.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    lcos::future<boost::uint32_t> get_num_localities_async(
        components::component_type type = components::component_invalid
        );

    boost::uint32_t get_num_localities(
        components::component_type type
      , error_code& ec = throws
        );

    boost::uint32_t get_num_localities(error_code& ec = throws)
    {
        return get_num_localities(components::component_invalid, ec);
    }

    lcos::future<boost::uint32_t> get_num_overall_threads_async();

    boost::uint32_t get_num_overall_threads(
        error_code& ec = throws
        );

    lcos::future<std::vector<boost::uint32_t> > get_num_threads_async();

    std::vector<boost::uint32_t> get_num_threads(
        error_code& ec = throws
        );

    /// \brief Return a unique id usable as a component type.
    ///
    /// This function returns the component type id associated with the
    /// given component name. If this is the first request for this
    /// component name a new unique id will be created.
    ///
    /// \param name       [in] The component name (string) to get the
    ///                   component type for.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \returns          The function returns the currently associated
    ///                   component type. Any error results in an
    ///                   exception thrown from this function.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    components::component_type get_component_id(
        std::string const& name
      , error_code& ec = throws
        );

    void iterate_types(
        iterate_types_function_type const& f
      , error_code& ec = throws
        );

    std::string get_component_type_name(
        components::component_type id
      , error_code& ec = throws
        );

    /// \brief Register a factory for a specific component type
    ///
    /// This function allows to register a component factory for a given
    /// locality and component type.
    ///
    /// \param locality_id  [in] The locality value uniquely identifying the
    ///                   given locality the factory needs to be registered
    ///                   for.
    /// \param name       [in] The component name (string) to register a
    ///                   factory for the given component type for.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \returns          The function returns the currently associated
    ///                   component type. Any error results in an
    ///                   exception thrown from this function. The returned
    ///                   component type is the same as if the function
    ///                   \a get_component_id was called using the same
    ///                   component name.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    components::component_type register_factory(
        naming::gid_type const& locality_id
      , std::string const& name
      , error_code& ec = throws
        )
    {
        return register_factory(naming::get_locality_id_from_gid(locality_id), name, ec);
    }

    components::component_type register_factory(
        boost::uint32_t locality_id
      , std::string const& name
      , error_code& ec = throws
        );

    /// \brief Get unique range of freely assignable global ids.
    ///
    /// Every locality needs to be able to assign global ids to different
    /// components without having to consult the AGAS server for every id
    /// to generate. This function can be called to preallocate a range of
    /// ids usable for this purpose.
    ///
    /// \param l          [in] The locality the locality id needs to be
    ///                   generated for. Repeating calls using the same
    ///                   locality results in identical locality_id values.
    /// \param count      [in] The number of global ids to be generated.
    /// \param lower_bound
    ///                   [out] The lower bound of the assigned id range.
    ///                   The returned value can be used as the first id
    ///                   to assign. This is valid only, if the return
    ///                   value of this function is true.
    /// \param upper_bound
    ///                   [out] The upper bound of the assigned id range.
    ///                   The returned value can be used as the last id
    ///                   to assign. This is valid only, if the return
    ///                   value of this function is true.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \returns          This function returns \a true if a new range has
    ///                   been generated (it has been called for the first
    ///                   time for the given locality) and returns \a false
    ///                   if this locality already got a range assigned in
    ///                   an earlier call. Any error results in an exception
    ///                   thrown from this function.
    ///
    /// \note             This function assigns a range of global ids usable
    ///                   by the given locality for newly created components.
    ///                   Any of the returned global ids still has to be
    ///                   bound to a local address, either by calling
    ///                   \a bind or \a bind_range.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    bool get_id_range(
        boost::uint64_t count
      , naming::gid_type& lower_bound
      , naming::gid_type& upper_bound
      , error_code& ec = throws
        );

    /// \brief Bind a global address to a local address.
    ///
    /// Every element in the HPX namespace has a unique global address
    /// (global id). This global address has to be associated with a concrete
    /// local address to be able to address an instance of a component using
    /// its global address.
    ///
    /// \param id         [in] The global address which has to be bound to
    ///                   the local address.
    /// \param addr       [in] The local address to be bound to the global
    ///                   address.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \returns          This function returns \a true, if this global id
    ///                   got associated with an local address. It returns
    ///                   \a false otherwise.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    ///
    /// \note             Binding a gid to a local address sets its global
    ///                   reference count to one.
    bool bind_local(
        naming::gid_type const& id
      , naming::address const& addr
      , error_code& ec = throws
        )
    {
        return bind_range_local(id, 1, addr, 0, ec);
    }

    hpx::future<bool> bind_async(
        naming::gid_type const& id
      , naming::address const& addr
      , boost::uint32_t locality_id
        )
    {
        return bind_range_async(id, 1, addr, 0,
            naming::get_gid_from_locality_id(locality_id));
    }

    hpx::future<bool> bind_async(
        naming::gid_type const& id
      , naming::address const& addr
      , naming::gid_type const& locality
        )
    {
        return bind_range_async(id, 1, addr, 0, locality);
    }

    /// \brief Bind unique range of global ids to given base address
    ///
    /// Every locality needs to be able to bind global ids to different
    /// components without having to consult the AGAS server for every id
    /// to bind. This function can be called to bind a range of consecutive
    /// global ids to a range of consecutive local addresses (separated by
    /// a given \a offset).
    ///
    /// \param lower_id   [in] The lower bound of the assigned id range.
    ///                   The value can be used as the first id to assign.
    /// \param count      [in] The number of consecutive global ids to bind
    ///                   starting at \a lower_id.
    /// \param baseaddr   [in] The local address to bind to the global id
    ///                   given by \a lower_id. This is the base address
    ///                   for all additional local addresses to bind to the
    ///                   remaining global ids.
    /// \param offset     [in] The offset to use to calculate the local
    ///                   addresses to be bound to the range of global ids.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \returns          This function returns \a true, if the given range
    ///                   was successfully bound. It returns \a false otherwise.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    ///
    /// \note             Binding a gid to a local address sets its global
    ///                   reference count to one.
    bool bind_range_local(
        naming::gid_type const& lower_id
      , boost::uint64_t count
      , naming::address const& baseaddr
      , boost::uint64_t offset
      , error_code& ec = throws
        );

    hpx::future<bool> bind_range_async(
        naming::gid_type const& lower_id
      , boost::uint64_t count
      , naming::address const& baseaddr
      , boost::uint64_t offset
      , naming::gid_type const& locality
        );

    hpx::future<bool> bind_range_async(
        naming::gid_type const& lower_id
      , boost::uint64_t count
      , naming::address const& baseaddr
      , boost::uint64_t offset
      , boost::uint32_t locality_id
        )
    {
        return bind_range_async(lower_id, count, baseaddr, offset,
            naming::get_gid_from_locality_id(locality_id));
    }

    /// \brief Unbind a global address
    ///
    /// Remove the association of the given global address with any local
    /// address, which was bound to this global address. Additionally it
    /// returns the local address which was bound at the time of this call.
    ///
    /// \param id         [in] The global address (id) for which the
    ///                   association has to be removed.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \returns          The function returns \a true if the association
    ///                   has been removed, and it returns \a false if no
    ///                   association existed. Any error results in an
    ///                   exception thrown from this function.
    ///
    /// \note             You can unbind only global ids bound using the
    ///                   function \a bind. Do not use this function to
    ///                   unbind any of the global ids bound using
    ///                   \a bind_range.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    ///
    /// \note             This function will raise an error if the global
    ///                   reference count of the given gid is not zero!
    ///                   TODO: confirm that this happens.
    bool unbind_local(
        naming::gid_type const& id
      , error_code& ec = throws
        )
    {
        return unbind_range_local(id, 1, ec);
    }

    /// \brief Unbind a global address
    ///
    /// Remove the association of the given global address with any local
    /// address, which was bound to this global address. Additionally it
    /// returns the local address which was bound at the time of this call.
    ///
    /// \param id         [in] The global address (id) for which the
    ///                   association has to be removed.
    /// \param addr       [out] The local address which was associated with
    ///                   the given global address (id).
    ///                   This is valid only if the return value of this
    ///                   function is true.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \returns          The function returns \a true if the association
    ///                   has been removed, and it returns \a false if no
    ///                   association existed. Any error results in an
    ///                   exception thrown from this function.
    ///
    /// \note             You can unbind only global ids bound using the
    ///                   function \a bind. Do not use this function to
    ///                   unbind any of the global ids bound using
    ///                   \a bind_range.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    ///
    /// \note             This function will raise an error if the global
    ///                   reference count of the given gid is not zero!
    ///                   TODO: confirm that this happens.
    bool unbind_local(
        naming::gid_type const& id
      , naming::address& addr
      , error_code& ec = throws
        )
    {
        return unbind_range_local(id, 1, addr, ec);
    }

    /// \brief Unbind the given range of global ids
    ///
    /// \param lower_id   [in] The lower bound of the assigned id range.
    ///                   The value must the first id of the range as
    ///                   specified to the corresponding call to
    ///                   \a bind_range.
    /// \param count      [in] The number of consecutive global ids to unbind
    ///                   starting at \a lower_id. This number must be
    ///                   identical to the number of global ids bound by
    ///                   the corresponding call to \a bind_range
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \returns          This function returns \a true if a new range has
    ///                   been generated (it has been called for the first
    ///                   time for the given locality) and returns \a false
    ///                   if this locality already got a range assigned in
    ///                   an earlier call. Any error results in an exception
    ///                   thrown from this function.
    ///
    /// \note             You can unbind only global ids bound using the
    ///                   function \a bind_range. Do not use this function
    ///                   to unbind any of the global ids bound using
    ///                   \a bind.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    ///
    /// \note             This function will raise an error if the global
    ///                   reference count of the given gid is not zero!
    ///                   TODO: confirm that this happens.
    bool unbind_range_local(
        naming::gid_type const& lower_id
      , boost::uint64_t count
      , error_code& ec = throws
        )
    {
        naming::address addr;
        return unbind_range_local(lower_id, count, addr, ec);
    }

    /// \brief Unbind the given range of global ids
    ///
    /// \param lower_id   [in] The lower bound of the assigned id range.
    ///                   The value must the first id of the range as
    ///                   specified to the corresponding call to
    ///                   \a bind_range.
    /// \param count      [in] The number of consecutive global ids to unbind
    ///                   starting at \a lower_id. This number must be
    ///                   identical to the number of global ids bound by
    ///                   the corresponding call to \a bind_range
    /// \param addr       [out] The local address which was associated with
    ///                   the given global address (id).
    ///                   This is valid only if the return value of this
    ///                   function is true.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \returns          This function returns \a true if a new range has
    ///                   been generated (it has been called for the first
    ///                   time for the given locality) and returns \a false
    ///                   if this locality already got a range assigned in
    ///                   an earlier call.
    ///
    /// \note             You can unbind only global ids bound using the
    ///                   function \a bind_range. Do not use this function
    ///                   to unbind any of the global ids bound using
    ///                   \a bind.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    ///
    /// \note             This function will raise an error if the global
    ///                   reference count of the given gid is not zero!
    bool unbind_range_local(
        naming::gid_type const& lower_id
      , boost::uint64_t count
      , naming::address& addr
      , error_code& ec = throws
        );

    hpx::future<naming::address> unbind_range_async(
        naming::gid_type const& lower_id
      , boost::uint64_t count = 1
        );

    /// \brief Test whether the given address refers to a local object.
    ///
    /// This function will test whether the given address refers to an object
    /// living on the locality of the caller.
    ///
    /// \param addr       [in] The address to test.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    /// \returns          This function returns \a true if the passed address
    ///                   refers to an object which lives on the locality of
    ///                   the caller.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
//     bool is_local_address(
//         naming::gid_type const& id
//       , error_code& ec = throws
//         )
//     {
//         naming::address addr;
//         return is_local_address(id, addr, ec);
//     }
//
//     bool is_local_address(
//         naming::gid_type const& id
//       , naming::address& addr
//       , error_code& ec = throws
//         );

    bool is_local_address_cached(
        naming::gid_type const& id
      , error_code& ec = throws
        )
    {
        naming::address addr;
        return is_local_address_cached(id, addr, ec);
    }

    bool is_local_address_cached(
        naming::gid_type const& id
      , naming::address& addr
      , error_code& ec = throws
        );

    bool is_local_lva_encoded_address(
        boost::uint64_t msb
        );

    // same, but bulk operation
//     bool is_local_address(
//         naming::gid_type const* gids
//       , naming::address* addrs
//       , std::size_t size
//       , boost::dynamic_bitset<>& locals
//       , error_code& ec = throws
//         );

    /// \brief Resolve a given global address (\a id) to its associated local
    ///        address.
    ///
    /// This function returns the local address which is currently associated
    /// with the given global address (\a id).
    ///
    /// \param id         [in] The global address (\a id) for which the
    ///                   associated local address should be returned.
    /// \param addr       [out] The local address which currently is
    ///                   associated with the given global address (id),
    ///                   this is valid only if the return value of this
    ///                   function is true.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \returns          This function returns \a true if the global
    ///                   address has been resolved successfully (there
    ///                   exists an association to a local address) and the
    ///                   associated local address has been returned. The
    ///                   function returns \a false if no association exists
    ///                   for the given global address. Any error results
    ///                   in an exception thrown from this function.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    bool resolve_local(
        naming::gid_type const& id
      , naming::address& addr
      , error_code& ec = throws
        )
    {
        // Try the cache
        if (caching_)
        {
            if (resolve_cached(id, addr, ec))
                return true;

            if (ec)
                return false;
        }

        return resolve_full_local(id, addr, ec);
    }

    bool resolve_local(
        naming::id_type const& id
      , naming::address& addr
      , error_code& ec = throws
        )
    {
        return resolve_local(id.get_gid(), addr, ec);
    }

    naming::address resolve_local(
        naming::gid_type const& id
      , error_code& ec = throws
        )
    {
        naming::address addr;
        resolve_local(id, addr, ec);
        return addr;
    }

    naming::address resolve_local(
        naming::id_type const& id
      , error_code& ec = throws
        )
    {
        naming::address addr;
        resolve_local(id.get_gid(), addr, ec);
        return addr;
    }

    ///////////////////////////////////////////////////////////////////////////
    hpx::future<naming::address> resolve_async(
        naming::gid_type const& id
        );

    hpx::future<naming::address> resolve_async(
        naming::id_type const& id
        )
    {
        return resolve_async(id.get_gid());
    }

    ///////////////////////////////////////////////////////////////////////////
    hpx::future<naming::id_type> get_colocation_id_async(
        naming::id_type const& id
        );

    ///////////////////////////////////////////////////////////////////////////
    bool resolve_full_local(
        naming::gid_type const& id
      , naming::address& addr
      , error_code& ec = throws
        );

    bool resolve_full_local(
        naming::id_type const& id
      , naming::address& addr
      , error_code& ec = throws
        )
    {
        return resolve_full_local(id.get_gid(), addr, ec);
    }

    naming::address resolve_full_local(
        naming::gid_type const& id
      , error_code& ec = throws
        )
    {
        naming::address addr;
        resolve_full_local(id, addr, ec);
        return addr;
    }

    naming::address resolve_full_local(
        naming::id_type const& id
      , error_code& ec = throws
        )
    {
        naming::address addr;
        resolve_full_local(id.get_gid(), addr, ec);
        return addr;
    }

    hpx::future<naming::address> resolve_full_async(
        naming::gid_type const& id
        );

    hpx::future<naming::address> resolve_full_async(
        naming::id_type const& id
        )
    {
        return resolve_full_async(id.get_gid());
    }

    bool resolve_cached(
        naming::gid_type const& id
      , naming::address& addr
      , error_code& ec = throws
        );

    bool resolve_cached(
        naming::id_type const& id
      , naming::address& addr
      , error_code& ec = throws
        )
    {
        return resolve_cached(id.get_gid(), addr, ec);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Bulk version.
    // TODO: Add versions that take std::vector<id_type> for convenience.
    bool resolve_local(
        naming::gid_type const* gids
      , naming::address* addrs
      , std::size_t size
      , boost::dynamic_bitset<>& locals
      , error_code& ec = throws
        )
    {
        // Try the cache.
        if (caching_)
        {
            bool all_resolved = resolve_cached(gids, addrs, size, locals, ec);
            if (ec)
                return false;
            if (all_resolved)
                return true; // Nothing more to do.
        }

        return resolve_full_local(gids, addrs, size, locals, ec);
    }

    bool resolve_full_local(
        naming::gid_type const* gids
      , naming::address* addrs
      , std::size_t size
      , boost::dynamic_bitset<>& locals
      , error_code& ec = throws
        );

    bool resolve_cached(
        naming::gid_type const* gids
      , naming::address* addrs
      , std::size_t size
      , boost::dynamic_bitset<>& locals
      , error_code& ec = throws
        );

    /// \brief Route the given parcel to the appropriate AGAS service instance
    ///
    /// This function sends the given parcel to the AGAS service instance which
    /// manages the parcel's destination GID. This service instance will resolve
    /// the GID and either send (route) the parcel to the correct locality or
    /// it will deliver the parcel to the local action manager.
    ///
    /// \param p          [in] this is the parcel which has to be routed to the
    ///                   AGAS service instance managing the destination GID.
    ///
    /// \note             The route operation is asynchronous, thus it returns
    ///                   before the parcel has been delivered to its
    ///                   destination.
    void route(
        parcelset::parcel p
      , util::function_nonser<void(boost::system::error_code const&,
            parcelset::parcel const&)> &&
      , threads::thread_priority local_priority =
            threads::thread_priority_default);

    /// \brief Increment the global reference count for the given id
    ///
    /// \param id         [in] The global address (id) for which the
    ///                   global reference count has to be incremented.
    /// \param credits    [in] The number of reference counts to add for
    ///                   the given id.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \returns          Whether the operation was successful.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    lcos::future<boost::int64_t> incref_async(
        naming::gid_type const& gid
      , boost::int64_t credits = 1
      , naming::id_type const& keep_alive = naming::invalid_id
        );

    boost::int64_t incref(
        naming::gid_type const& gid
      , boost::int64_t credits = 1
      , error_code& ec = throws
        )
    {
        return incref_async(gid, credits).get(ec);
    }

    /// \brief Decrement the global reference count for the given id
    ///
    /// \param id         [in] The global address (id) for which the
    ///                   global reference count has to be decremented.
    /// \param t          [out] If this was the last outstanding global
    ///                   reference for the given gid (the return value of
    ///                   this function is zero), t will be set to the
    ///                   component type of the corresponding element.
    ///                   Otherwise t will not be modified.
    /// \param credits    [in] The number of reference counts to add for
    ///                   the given id.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \returns          The global reference count after the decrement.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    void decref(
        naming::gid_type const& id
      , boost::int64_t credits = 1
      , error_code& ec = throws
        );

    /// \brief Invoke the supplied \a hpx#function for every registered global
    ///        name.
    ///
    /// This function iterates over all registered global ids and
    /// unconditionally invokes the supplied hpx#function for ever found entry.
    /// Any error results in an exception thrown (or reported) from this
    /// function.
    ///
    /// \param f          [in] a \a hpx#function encapsulating an action to be
    ///                   invoked for every currently registered global name.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    bool iterate_ids(
        iterate_names_function_type const& f
      , error_code& ec = throws
        );

    /// \brief Register a global name with a global address (id)
    ///
    /// This function registers an association between a global name
    /// (string) and a global address (id) usable with one of the functions
    /// above (bind, unbind, and resolve).
    ///
    /// \param name       [in] The global name (string) to be associated
    ///                   with the global address.
    /// \param id         [in] The global address (id) to be associated
    ///                   with the global address.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \returns          The function returns \a true if the global name
    ///                   was registered. It returns false if the global name is
    ///                   not registered.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    bool register_name(
        std::string const& name
      , naming::gid_type const& id
      , error_code& ec = throws
        );

    lcos::future<bool> register_name_async(
        std::string const& name
      , naming::id_type const& id
        );

    bool register_name(
        std::string const& name
      , naming::id_type const& id
      , error_code& ec = throws
        )
    {
        return register_name_async(name, id).get(ec);
    }

    /// \brief Unregister a global name (release any existing association)
    ///
    /// This function releases any existing association of the given global
    /// name with a global address (id).
    ///
    /// \param name       [in] The global name (string) for which any
    ///                   association with a global address (id) has to be
    ///                   released.
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// \returns          The function returns \a true if an association of
    ///                   this global name has been released, and it returns
    ///                   \a false, if no association existed. Any error
    ///                   results in an exception thrown from this function.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    bool unregister_name(
        std::string const& name
      , naming::gid_type& id
      , error_code& ec = throws
        );

    lcos::future<naming::id_type> unregister_name_async(
        std::string const& name
        );

    naming::id_type unregister_name(
        std::string const& name
      , error_code& ec = throws
        )
    {
        return unregister_name_async(name).get(ec);
    }

    /// \brief Query for the global address associated with a given global name.
    ///
    /// This function returns the global address associated with the given
    /// global name.
    ///
    /// \param name       [in] The global name (string) for which the
    ///                   currently associated global address has to be
    ///                   retrieved.
    /// \param id         [out] The id currently associated with the given
    ///                   global name (valid only if the return value is
    ///                   true).
    /// \param ec         [in,out] this represents the error status on exit,
    ///                   if this is pre-initialized to \a hpx#throws
    ///                   the function will throw on error instead.
    ///
    /// This function returns true if it returned global address (id),
    /// which is currently associated with the given global name, and it
    /// returns false, if currently there is no association for this global
    /// name. Any error results in an exception thrown from this function.
    ///
    /// \note             As long as \a ec is not pre-initialized to
    ///                   \a hpx#throws this function doesn't
    ///                   throw but returns the result code using the
    ///                   parameter \a ec. Otherwise it throws an instance
    ///                   of hpx#exception.
    bool resolve_name(
        std::string const& name
      , naming::gid_type& id
      , error_code& ec = throws
        );

    lcos::future<naming::id_type> resolve_name_async(
        std::string const& name
        );

    naming::id_type resolve_name(
        std::string const& name
      , error_code& ec = throws
        )
    {
        return resolve_name_async(name).get(ec);
    }

    /// \brief Install a listener for a given symbol namespace event.
    ///
    /// This function installs a listener for a given symbol namespace event.
    /// It returns a future which becomes ready as a result of the listener
    /// being triggered.
    ///
    /// \param name       [in] The global name (string) for which the given
    ///                   event should be triggered.
    /// \param evt        [in] The event for which a listener should be
    ///                   installed.
    /// \param call_for_past_events   [in, optional] Trigger the listener even
    ///                   if the given event has already happened in the past.
    ///                   The default for this parameter is \a false.
    ///
    /// \returns  A future instance encapsulating the global id which is
    ///           causing the registered listener to be triggered.
    ///
    /// \note    The only event type which is currently supported is
    ///          \a symbol_ns_bind, i.e. the listener is triggered whenever a
    ///          global id is registered with the given name.
    ///
    future<hpx::id_type> on_symbol_namespace_event(std::string const& name,
        namespace_action_code evt, bool call_for_past_events = false);

    /// \warning This function is for internal use only. It is dangerous and
    ///          may break your code if you use it.
    void update_cache_entry(
        naming::gid_type const& gid
      , gva const& gva
      , error_code& ec = throws
        );

    /// \warning This function is for internal use only. It is dangerous and
    ///          may break your code if you use it.
    void update_cache_entry(
        naming::gid_type const& gid
      , naming::address const& addr
      , boost::uint64_t count = 0
      , boost::uint64_t offset = 0
      , error_code& ec = throws
        )
    {
        const gva g(addr.locality_, addr.type_, count, addr.address_, offset);
        update_cache_entry(gid, g, ec);
    }

    /// \warning This function is for internal use only. It is dangerous and
    ///          may break your code if you use it.
    bool get_cache_entry(
        naming::gid_type const& gid
      , gva& gva
      , naming::gid_type& idbase
      , error_code& ec = throws
        );

    /// \warning This function is for internal use only. It is dangerous and
    ///          may break your code if you use it.
    void remove_cache_entry(
        naming::gid_type const& id
      , error_code& ec = throws
        );

    /// \warning This function is for internal use only. It is dangerous and
    ///          may break your code if you use it.
    void clear_cache(
        error_code& ec = throws
        );

    // Disable refcnt caching during shutdown
    void start_shutdown(
        error_code& ec = throws
        );

    /// \brief Retrieve statistics performance counter
    bool retrieve_statistics_counter(
        std::string const& counter_name
      , naming::gid_type& counter
      , error_code& ec = throws
        );

    /// start/stop migration of an object
    ///
    /// \returns Current locality and address of the object to migrate
    hpx::future<std::pair<naming::id_type, naming::address> >
        begin_migration_async(naming::id_type const& id);
    hpx::future<bool> end_migration_async(naming::id_type const& id);

    /// Maintain list of migrated objects
    std::pair<bool, components::pinned_ptr>
        was_object_migrated(naming::gid_type const& gid,
            util::unique_function_nonser<components::pinned_ptr()> && f);

    /// Mark the given object as being migrated (if the object is unpinned).
    /// Delay migration until the object is unpinned otherwise.
    hpx::future<void> mark_as_migrated(naming::gid_type const& gid,
        util::unique_function_nonser<
            std::pair<bool, hpx::future<void> >()> && f);

    /// Remove the given object from the table of migrated objects
    void unmark_as_migrated(naming::gid_type const& gid);
};

}}

#endif // HPX_15D904C7_CD18_46E1_A54A_65059966A34F

