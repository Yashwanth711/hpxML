//  Copyright (c) 2007-2015 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file parallel/executors/timed_executor_traits.hpp

#if !defined(HPX_PARALLEL_TIMED_EXECUTOR_TRAITS_AUG_04_2015_0525PM)
#define HPX_PARALLEL_TIMED_EXECUTOR_TRAITS_AUG_04_2015_0525PM

#include <hpx/config.hpp>
#include <hpx/exception.hpp>
#include <hpx/async.hpp>
#include <hpx/traits/is_timed_executor.hpp>
#include <hpx/lcos/future.hpp>
#include <hpx/util/date_time_chrono.hpp>
#include <hpx/parallel/config/inline_namespace.hpp>
#include <hpx/parallel/executors/executor_traits.hpp>
#include <hpx/util/date_time_chrono.hpp>

#include <type_traits>
#include <utility>

#include <boost/range/functions.hpp>
#include <boost/range/irange.hpp>

namespace hpx { namespace parallel { HPX_INLINE_NAMESPACE(v3)
{
    namespace detail
    {
        /// \cond NOINTERNAL

        ///////////////////////////////////////////////////////////////////////
        template <typename Tag>
        struct apply_execute_at_helper
        {
            template <typename Executor, typename F>
            static void call(wrap_int, Executor& exec,
                util::steady_time_point const& abs_time, F && f)
            {
                make_ready_future_at(abs_time).then(
                    [exec, f](hpx::future<void>) mutable
                    {
                        call_apply_execute(exec, std::move(f));
                    });
            }

            template <typename Executor, typename F>
            static auto call(int, Executor& exec,
                    util::steady_time_point const& abs_time, F && f)
            ->  decltype(exec.apply_execute_at(abs_time, std::forward<F>(f)))
            {
                return exec.apply_execute_at(abs_time, std::forward<F>(f));
            }
        };

        template <>
        struct apply_execute_at_helper<sequential_execution_tag>
        {
            template <typename Executor, typename F>
            static void call(wrap_int, Executor& exec,
                util::steady_time_point const& abs_time, F && f)
            {
                this_thread::sleep_until(abs_time);
                call_apply_execute(exec, std::forward<F>(f));
            }

            template <typename Executor, typename F>
            static auto call(int, Executor& exec,
                    util::steady_time_point const& abs_time, F && f)
            ->  decltype(exec.apply_execute_at(abs_time, std::forward<F>(f)))
            {
                exec.apply_execute_at(abs_time, std::forward<F>(f));
            }
        };

        template <typename Executor, typename F>
        void call_apply_execute_at(Executor& exec,
            util::steady_time_point const& abs_time, F && f)
        {
            typedef typename detail::execution_category<Executor>::type tag;
            return apply_execute_at_helper<tag>::call(0, exec, abs_time,
                std::forward<F>(f));
        }

        ///////////////////////////////////////////////////////////////////////
        template <typename Tag>
        struct async_execute_at_helper
        {
            template <typename Executor, typename F>
            static auto call(wrap_int, Executor& exec,
                    util::steady_time_point const& abs_time, F && f)
            ->  decltype(exec.async_execute(std::forward<F>(f)))
            {
                return make_ready_future_at(abs_time).then(
                    [exec, f](hpx::future<void>) mutable
                    {
                        return exec.async_execute(std::move(f));
                    });
            }

            template <typename Executor, typename F>
            static auto call(int, Executor& exec,
                    util::steady_time_point const& abs_time, F && f)
            ->  decltype(exec.async_execute_at(abs_time, std::forward<F>(f)))
            {
                return exec.async_execute_at(abs_time, std::forward<F>(f));
            }
        };

        template <>
        struct async_execute_at_helper<sequential_execution_tag>
        {
            template <typename Executor, typename F>
            static auto call(wrap_int, Executor& exec,
                    util::steady_time_point const& abs_time, F && f)
            ->  decltype(exec.async_execute(std::forward<F>(f)))
            {
                this_thread::sleep_until(abs_time);
                return exec.async_execute(std::forward<F>(f));
            }

            template <typename Executor, typename F>
            static auto call(int, Executor& exec,
                    util::steady_time_point const& abs_time, F && f)
            ->  decltype(exec.async_execute_at(abs_time, std::forward<F>(f)))
            {
                return exec.async_execute_at(abs_time, std::forward<F>(f));
            }
        };

        template <typename Executor, typename F>
        auto call_async_execute_at(Executor& exec,
                util::steady_time_point const& abs_time, F && f)
        ->  decltype(
                async_execute_at_helper<
                        typename detail::execution_category<Executor>::type
                    >::call(0, exec, abs_time, std::forward<F>(f))
            )
        {
            typedef typename detail::execution_category<Executor>::type tag;
            return async_execute_at_helper<tag>::call(0, exec, abs_time,
                std::forward<F>(f));
        }

        ///////////////////////////////////////////////////////////////////////
        template <typename Tag>
        struct execute_at_helper
        {
            template <typename Executor, typename F>
            static auto call(wrap_int, Executor& exec,
                    util::steady_time_point const& abs_time, F && f)
            ->  decltype(call_execute(exec, std::forward<F>(f)))
            {
                return make_ready_future_at(abs_time).then(
                    [exec, f](hpx::future<void>) mutable
                    {
                        return call_execute(exec, std::move(f));
                    }).get();
            }

            template <typename Executor, typename F>
            static auto call(int, Executor& exec,
                    util::steady_time_point const& abs_time, F && f)
            ->  decltype(exec.execute_at(abs_time, std::forward<F>(f)))
            {
                return exec.execute_at(abs_time, std::forward<F>(f));
            }
        };

        template <>
        struct execute_at_helper<sequential_execution_tag>
        {
            template <typename Executor, typename F>
            static auto call(wrap_int, Executor& exec,
                    util::steady_time_point const& abs_time, F && f)
            ->  decltype(call_execute(exec, std::forward<F>(f)))
            {
                this_thread::sleep_until(abs_time);
                return call_execute(exec, std::forward<F>(f));
            }

            template <typename Executor, typename F>
            static auto call(int, Executor& exec,
                    util::steady_time_point const& abs_time, F && f)
            ->  decltype(exec.execute_at(abs_time, std::forward<F>(f)))
            {
                return exec.execute_at(abs_time, std::forward<F>(f));
            }
        };

        template <typename Executor, typename F>
        auto call_execute_at(Executor& exec,
                util::steady_time_point const& abs_time, F && f)
        ->  decltype(
                execute_at_helper<
                        typename detail::execution_category<Executor>::type
                    >::call(0, exec, abs_time, std::forward<F>(f))
            )
        {
            typedef typename detail::execution_category<Executor>::type tag;
            return execute_at_helper<tag>::call(0, exec, abs_time,
                std::forward<F>(f));
        }

        /// \endcond
    }

    ///////////////////////////////////////////////////////////////////////////
    /// The timed_executor_traits type is used to request execution agents from
    /// an executor. It is analogous to the interaction between containers and
    /// allocator_traits. The generated generated execution agents support
    /// timed scheduling functionality (in addition to what is supported
    /// execution agents generated using execution_traits type).
    ///
    template <typename Executor, typename Enable>
    struct timed_executor_traits : executor_traits<Executor>
    {
        /// The type of the executor associated with this instance of
        /// \a executor_traits
        typedef typename executor_traits<Executor>::executor_type
            executor_type;

        /// The category of agents created by the bulk-form execute() and
        /// async_execute()
        ///
        /// \note This evaluates to executor_type::execution_category if it
        ///       exists; otherwise it evaluates to \a parallel_execution_tag.
        ///
        typedef typename executor_traits<Executor>::execution_category
            execution_category;

        /// The type of future returned by async_execute()
        ///
        /// \note This evaluates to executor_type::future_type<T> if it exists;
        ///       otherwise it evaluates to \a hpx::future<T>
        ///
        template <typename T>
        struct future
        {
            /// The future type returned from async_execute
            typedef typename detail::future_type<executor_type, T>::type type;
        };

        /// \brief Singleton form of asynchronous fire & forget execution agent
        ///        creation supporting timed execution.
        ///
        /// This asynchronously (fire & forget) creates a single function
        /// invocation f() using the associated executor at the given point in
        /// time.
        ///
        /// \param exec [in] The executor object to use for scheduling of the
        ///             function \a f.
        /// \param abs_time [in] The point in time the given function should be
        ///             scheduled at to run.
        /// \param f    [in] The function which will be scheduled using the
        ///             given executor.
        ///
        /// \note This calls exec.apply_execute_at(abs_time, f), if available,
        ///       otherwise it emulates timed scheduling by delaying calling
        ///       exec.apply_execute() on the underlying non-scheduled
        ///       execution agent while discarding the returned future.
        ///
        template <typename F>
        static void apply_execute_at(executor_type& exec,
            hpx::util::steady_time_point const& abs_time, F && f)
        {
            detail::call_apply_execute_at(exec, abs_time, std::forward<F>(f));
        }

        /// \brief Singleton form of asynchronous fire & forget execution agent
        ///        creation supporting timed execution.
        ///
        /// This asynchronously (fire & forget) creates a single function
        /// invocation f() using the associated executor after the given amount
        /// of time.
        ///
        /// \param exec [in] The executor object to use for scheduling of the
        ///             function \a f.
        /// \param rel_time [in] The duration of time after which the given
        ///             function should be scheduled to run.
        /// \param f    [in] The function which will be scheduled using the
        ///             given executor.
        ///
        /// \note This calls exec.apply_execute_at(abs_time, f), if available,
        ///       otherwise it emulates timed scheduling by delaying calling
        ///       exec.apply_execute() on the underlying non-scheduled
        ///       execution agent while discarding the returned future.
        ///
        template <typename F>
        static void apply_execute_after(executor_type& exec,
            hpx::util::steady_duration const& rel_time, F && f)
        {
            detail::call_apply_execute_at(exec, rel_time.from_now(),
                std::forward<F>(f));
        }

        /// \brief Singleton form of asynchronous execution agent creation
        ///        supporting timed execution.
        ///
        /// This asynchronously creates a single function invocation f() using
        /// the associated executor at the given point in time.
        ///
        /// \param exec [in] The executor object to use for scheduling of the
        ///             function \a f.
        /// \param abs_time [in] The point in time the given function should be
        ///             scheduled at to run.
        /// \param f    [in] The function which will be scheduled using the
        ///             given executor.
        ///
        /// \note This calls exec.async_execute_at(abs_time, f), if available,
        ///       otherwise it emulates timed scheduling by delaying calling
        ///       exec.async_execute() on the underlying non-scheduled
        ///       execution agent.
        ///
        /// \returns f()'s result through a future
        ///
        template <typename F>
        static auto async_execute_at(executor_type& exec,
                hpx::util::steady_time_point const& abs_time, F && f)
        ->  decltype(
                detail::call_async_execute_at(exec, abs_time,
                    std::forward<F>(f))
            )
        {
            return detail::call_async_execute_at(exec, abs_time,
                std::forward<F>(f));
        }

        /// \brief Singleton form of asynchronous execution agent creation
        ///        supporting timed execution.
        ///
        /// This asynchronously creates a single function invocation f() using
        /// the associated executor after the given amount of time.
        ///
        /// \param exec [in] The executor object to use for scheduling of the
        ///             function \a f.
        /// \param rel_time [in] The duration of time after which the given
        ///             function should be scheduled to run.
        /// \param f    [in] The function which will be scheduled using the
        ///             given executor.
        ///
        /// \note This calls exec.async_execute_at(abs_time, f), if available,
        ///       otherwise it emulates timed scheduling by delaying calling
        ///       exec.async_execute() on the underlying non-scheduled
        ///       execution agent.
        ///
        /// \returns f()'s result through a future
        ///
        template <typename F>
        static auto async_execute_after(executor_type& exec,
                hpx::util::steady_duration const& rel_time, F && f)
        ->  decltype(
                detail::call_async_execute_at(exec, rel_time.from_now(),
                    std::forward<F>(f))
            )
        {
            return detail::call_async_execute_at(exec, rel_time.from_now(),
                std::forward<F>(f));
        }

        /// \brief Singleton form of synchronous execution agent creation
        ///        supporting timed execution.
        ///
        /// This synchronously creates a single function invocation f() using
        /// the associated executor at the given point in time. The execution
        /// of the supplied function synchronizes with the caller.
        ///
        /// \param exec [in] The executor object to use for scheduling of the
        ///             function \a f.
        /// \param abs_time [in] The point in time the given function should be
        ///             scheduled at to run.
        /// \param f    [in] The function which will be scheduled using the
        ///             given executor.
        ///
        /// \returns f()'s result
        ///
        /// \note This calls exec.execute(f) if it exists;
        ///       otherwise it emulates timed scheduling by delaying calling
        ///       exec.execute() on the underlying non-scheduled
        ///       execution agent.
        ///
        template <typename F>
        static auto
        execute_at(executor_type& exec,
                hpx::util::steady_time_point const& abs_time, F && f)
        ->  decltype(detail::call_execute_at(exec, abs_time, std::forward<F>(f)))
        {
            return detail::call_execute_at(exec, abs_time, std::forward<F>(f));
        }

        /// \brief Singleton form of synchronous execution agent creation
        ///        supporting timed execution.
        ///
        /// This synchronously creates a single function invocation f() using
        /// the associated executor after the given amount of time. The
        /// execution of the supplied function synchronizes with the caller.
        ///
        /// \param exec [in] The executor object to use for scheduling of the
        ///             function \a f.
        /// \param rel_time [in] The duration of time after which the given
        ///             function should be scheduled to run.
        /// \param f    [in] The function which will be scheduled using the
        ///             given executor.
        ///
        /// \returns f()'s result
        ///
        /// \note This calls exec.execute(f) if it exists;
        ///       otherwise it emulates timed scheduling by delaying calling
        ///       exec.execute() on the underlying non-scheduled
        ///       execution agent.
        ///
        template <typename F>
        static auto
        execute_after(executor_type& exec,
                hpx::util::steady_duration const& rel_time, F && f)
        ->  decltype(
                detail::call_execute_at(exec, rel_time.from_now(),
                    std::forward<F>(f))
            )
        {
            return detail::call_execute_at(exec, rel_time.from_now(),
                std::forward<F>(f));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    /// 1. The type is_timed_executor can be used to detect executor types
    ///    for the purpose of excluding function signatures
    ///    from otherwise ambiguous overload resolution participation.
    /// 2. If T is the type of a standard or implementation-defined executor,
    ///    is_timed_executor<T> shall be publicly derived from
    ///    integral_constant<bool, true>, otherwise from
    ///    integral_constant<bool, false>.
    /// 3. The behavior of a program that adds specializations for
    ///    is_timed_executor is undefined.
    ///
    template <typename T>
    struct is_timed_executor;   // defined in hpx/traits/is_timed_executor.hpp
}}}

#endif
