
#ifndef HPX_LCOS_FWD_HPP
#define HPX_LCOS_FWD_HPP

#include <hpx/config.hpp>
#include <hpx/traits/promise_remote_result.hpp>

namespace hpx
{
    /// \namespace lcos
    namespace lcos
    {
        namespace detail
        {
            struct future_data_refcnt_base;
        }

        class base_lco;
        template <typename Result, typename RemoteResult = Result>
        class base_lco_with_value;

        template <typename Result,
            typename RemoteResult =
                typename traits::promise_remote_result<Result>::type>
        class promise;

        template <typename Action,
            typename Result = typename traits::promise_local_result<
                typename Action::remote_result_type>::type,
            typename DirectExecute = typename Action::direct_execution>
        class packaged_action;

        template <typename R>
        class future;

        template <typename R>
        class shared_future;

        template <typename ValueType>
        struct object_semaphore;

        namespace stubs
        {
            template <typename ValueType>
            struct object_semaphore;
        }

        namespace server
        {
            template <typename ValueType>
            struct object_semaphore;
        }

        namespace local
        {
            class barrier;

            template <typename R>
            class promise;
        }
    }

    using lcos::future;
    using lcos::shared_future;
    using lcos::promise;
}

#endif
