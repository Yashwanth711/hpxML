//  Copyright (c) 2007-2012 Hartmut Kaiser
//  Copyright (c) 2013 Agustin Berge
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_TRAITS_IS_CALLABLE_APR_15_2012_0601PM)
#define HPX_TRAITS_IS_CALLABLE_APR_15_2012_0601PM

#include <hpx/config.hpp>

#if defined(BOOST_NO_SFINAE_EXPR)                                              \
 || defined(BOOST_NO_CXX11_DECLTYPE_N3276)                                     \
 || defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) // C++03

#include <hpx/traits/is_action.hpp>
#include <hpx/util/decay.hpp>
#include <hpx/util/detail/pp_strip_parens.hpp>
#include <hpx/util/detail/qualify_as.hpp>

#include <boost/function_types/components.hpp>
#include <boost/function_types/function_pointer.hpp>
#include <boost/move/move.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/facilities/intercept.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/ref.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/type_traits/is_class.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility/declval.hpp>

#include <cstddef>

// The technique implemented here was devised by Eric Niebler, see:
// http://www.boost.org/doc/libs/1_54_0/doc/html/proto/appendices.html#boost_proto.appendices.implementation.function_arity
namespace hpx { namespace traits
{
    namespace detail
    {
        struct fallback_argument
        {
            template <typename T> fallback_argument(T const&);
        };

        struct fallback_call
        {
            fallback_call const& operator,(int) const volatile;
        };

        template <typename T, std::size_t Arity>
        struct callable_wrapper_fallback;

        template <typename R>
        struct callable_wrapper_fallback<R(*)(), 0>
        {};

#       define HPX_TRAITS_DECL_FALLBACK(z, n, data)                             \
        template <typename T>                                                   \
        struct callable_wrapper_fallback<T, n>                                  \
        {                                                                       \
            typedef fallback_call const& (*pointer_to_function)(                \
                    BOOST_PP_ENUM_PARAMS(n, fallback_argument BOOST_PP_INTERCEPT)\
                );                                                              \
            operator pointer_to_function() const volatile;                      \
        };                                                                      \
        /**/

        BOOST_PP_REPEAT(HPX_PP_ROUND_UP_ADD3(HPX_TUPLE_LIMIT)
          , HPX_TRAITS_DECL_FALLBACK, _);

#       undef HPX_TRAITS_DECL_FALLBACK

        template <typename T, std::size_t Arity, typename Enable = void>
        struct callable_wrapper
          : callable_wrapper_fallback<T, Arity>
        {};

        template <typename T, std::size_t Arity>
        struct callable_wrapper<T, Arity
          , typename boost::enable_if<
                boost::mpl::and_<
                    boost::is_class<T>
                  , boost::mpl::not_<boost::is_reference_wrapper<T> >
                >
            >::type
        > : T
          , callable_wrapper_fallback<T, Arity>
        {};

        template <typename T, std::size_t Arity>
        struct callable_wrapper<boost::reference_wrapper<T>, Arity>
          : callable_wrapper<typename util::decay<T>::type, Arity>
        {};

#       define HPX_TRAITS_DECL_CALLABLE_WRAPPER_FUNCTION(z, n, data)            \
        template <typename R                                                    \
            BOOST_PP_ENUM_TRAILING_PARAMS(n, typename P)                        \
          , std::size_t Arity>                                                  \
        struct callable_wrapper<R(*)(BOOST_PP_ENUM_PARAMS(n, P)), Arity>        \
          : callable_wrapper_fallback<R(*)(BOOST_PP_ENUM_PARAMS(n, P)), Arity>  \
        {                                                                       \
            R operator()(BOOST_PP_ENUM_PARAMS(n, P)) const volatile;            \
        };                                                                      \
                                                                                \
        template <typename R, typename C                                        \
            BOOST_PP_ENUM_TRAILING_PARAMS(n, typename P)                        \
          , std::size_t Arity>                                                  \
        struct callable_wrapper<R(C::*)(BOOST_PP_ENUM_PARAMS(n, P)), Arity>     \
          : callable_wrapper_fallback<R(C::*)(BOOST_PP_ENUM_PARAMS(n, P)), Arity>\
        {                                                                       \
            R operator()(C*                                                     \
                BOOST_PP_ENUM_TRAILING_PARAMS(n, P)) const volatile;            \
            R operator()(boost::intrusive_ptr<C>                                \
                BOOST_PP_ENUM_TRAILING_PARAMS(n, P)) const volatile;            \
            R operator()(boost::shared_ptr<C>                                   \
                BOOST_PP_ENUM_TRAILING_PARAMS(n, P)) const volatile;            \
            R operator()(C&                                                     \
                BOOST_PP_ENUM_TRAILING_PARAMS(n, P)) const volatile;            \
            R operator()(BOOST_RV_REF(C)                                        \
                BOOST_PP_ENUM_TRAILING_PARAMS(n, P)) const volatile;            \
        };                                                                      \
                                                                                \
        template <typename R, typename C                                        \
            BOOST_PP_ENUM_TRAILING_PARAMS(n, typename P)                        \
          , std::size_t Arity>                                                  \
        struct callable_wrapper<R(C::*)(BOOST_PP_ENUM_PARAMS(n, P)) const, Arity>\
          : callable_wrapper_fallback<R(C::*)(BOOST_PP_ENUM_PARAMS(n, P)) const, Arity>\
        {                                                                       \
            R operator()(C const*                                               \
                BOOST_PP_ENUM_TRAILING_PARAMS(n, P)) const volatile;            \
            R operator()(boost::intrusive_ptr<C const>                          \
                BOOST_PP_ENUM_TRAILING_PARAMS(n, P)) const volatile;            \
            R operator()(boost::shared_ptr<C const>                             \
                BOOST_PP_ENUM_TRAILING_PARAMS(n, P)) const volatile;            \
            R operator()(C const&                                               \
                BOOST_PP_ENUM_TRAILING_PARAMS(n, P)) const volatile;            \
            R operator()(BOOST_RV_REF(C const)                                  \
                BOOST_PP_ENUM_TRAILING_PARAMS(n, P)) const volatile;            \
        };                                                                      \
        /**/

        BOOST_PP_REPEAT(HPX_PP_ROUND_UP_ADD3(HPX_TUPLE_LIMIT)
          , HPX_TRAITS_DECL_CALLABLE_WRAPPER_FUNCTION, _);

#       undef HPX_TRAITS_DECL_CALLABLE_WRAPPER_FUNCTION

        template <typename M, typename C, std::size_t Arity>
        struct callable_wrapper<M C::*, Arity>
          : callable_wrapper_fallback<M C::*, Arity>
        {
            M& operator()(C const*) const volatile;
            M& operator()(boost::intrusive_ptr<C const>) const volatile;
            M& operator()(boost::shared_ptr<C const>) const volatile;
            M& operator()(C) const volatile;
        };

        template <typename T
          , BOOST_PP_ENUM_BINARY_PARAMS(HPX_PP_ROUND_UP_ADD3(HPX_TUPLE_LIMIT)
              , typename A, = void BOOST_PP_INTERCEPT)
          , typename Dummy = void>
        struct is_callable_impl;

#       define HPX_TRAITS_DECL_IS_CALLABLE_IMPL(z, n, data)                     \
        template <typename F BOOST_PP_ENUM_TRAILING_PARAMS(n, typename T)>      \
        struct is_callable_impl<F BOOST_PP_ENUM_TRAILING_PARAMS(n, T)>          \
        {                                                                       \
            typedef typename util::detail::qualify_as<                          \
                    detail::callable_wrapper<typename util::decay<F>::type, n>, F\
                >::type callable_wrapper;                                       \
            typedef char (&no_type)[1];                                         \
            typedef char (&yes_type)[2];                                        \
                                                                                \
            template<typename T>                                                \
            static yes_type can_be_called(T const &);                           \
            static no_type can_be_called(detail::fallback_call const &);        \
                                                                                \
            static bool const value =                                           \
                sizeof(can_be_called((boost::declval<callable_wrapper>()(       \
                    BOOST_PP_ENUM_BINARY_PARAMS(n,                              \
                        boost::declval<T, >() BOOST_PP_INTERCEPT)), 0))         \
                ) == sizeof(yes_type);                                          \
            typedef boost::mpl::bool_<value> type;                              \
        };                                                                      \
        /**/

        BOOST_PP_REPEAT(HPX_PP_ROUND_UP_ADD3(HPX_TUPLE_LIMIT)
          , HPX_TRAITS_DECL_IS_CALLABLE_IMPL, _);

#       undef HPX_TRAITS_DECL_IS_CALLABLE_IMPL
    }

    template <typename T
      , BOOST_PP_ENUM_BINARY_PARAMS(HPX_PP_ROUND_UP_ADD3(HPX_TUPLE_LIMIT)
          , typename A, = void BOOST_PP_INTERCEPT)>
    struct is_callable
      : detail::is_callable_impl<
            T, BOOST_PP_ENUM_PARAMS(HPX_PP_ROUND_UP_ADD3(HPX_TUPLE_LIMIT), A)
        >
    {};
}}

#else // C++11

#include <hpx/util/always_void.hpp>
#include <hpx/util/decay.hpp>

#include <boost/get_pointer.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/ref.hpp>
#include <boost/utility/declval.hpp>
#include <boost/utility/enable_if.hpp>

namespace hpx { namespace traits
{
    namespace detail
    {
        using boost::get_pointer;

        template <typename T, typename Args, typename Enable = void>
        struct is_callable_impl
          : boost::mpl::false_
        {};

        template <typename T, typename... A>
        struct is_callable_impl<T, void(A...)
          , typename util::always_void<decltype(
                boost::declval<T>()(boost::declval<A>()...)
            )>::type
        > : boost::mpl::true_
        {};

        template <typename T, typename C>
        struct is_callable_impl<T, void(C)
          , typename util::always_void<decltype(
                boost::declval<C>().*boost::declval<T>()
            )>::type
        > : boost::mpl::true_
        {};
        template <typename T, typename C>
        struct is_callable_impl<T, void(C)
          , typename util::always_void<decltype(
                (*get_pointer(boost::declval<C>())).*boost::declval<T>()
            )>::type
        > : boost::mpl::true_
        {};

        template <typename T, typename C, typename... A>
        struct is_callable_impl<T, void(C, A...)
          , typename util::always_void<decltype(
                (boost::declval<C>().*boost::declval<T>())
                    (boost::declval<A>()...)
            )>::type
        > : boost::mpl::true_
        {};
        template <typename T, typename C, typename... A>
        struct is_callable_impl<T, void(C, A...)
          , typename util::always_void<decltype(
                ((*get_pointer(boost::declval<C>())).*boost::declval<T>())
                    (boost::declval<A>()...)
            )>::type
        > : boost::mpl::true_
        {};

        // support boost::[c]ref, which is not callable as std::[c]ref
        template <typename T, typename Args>
        struct is_callable_impl<T, Args
          , typename boost::enable_if_c<
                boost::is_reference_wrapper<
                    typename util::decay<T>::type
                >::value
            >::type
        > : is_callable_impl<
                typename boost::unwrap_reference<
                    typename util::decay<T>::type
                >::type&
              , Args
            >
        {};
    }

    template <typename T, typename... A>
    struct is_callable
      : detail::is_callable_impl<T, void(A...)>
    {};
}}

#endif

#include <hpx/traits/is_action.hpp>
#include <hpx/util/decay.hpp>

#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>

#include <boost/preprocessor/facilities/intercept.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

namespace hpx { namespace traits { namespace detail
{
    template <typename T
      , BOOST_PP_ENUM_BINARY_PARAMS(HPX_PP_ROUND_UP_ADD3(HPX_TUPLE_LIMIT)
          , typename A, = void BOOST_PP_INTERCEPT)
      , typename Dummy = void>
    struct is_callable_not_action;

#   define HPX_TRAITS_DECL_IS_CALLABLE_NOT_ACTION(z, n, data)                   \
    template <typename T BOOST_PP_ENUM_TRAILING_PARAMS(n, typename A)>          \
    struct is_callable_not_action<T BOOST_PP_ENUM_TRAILING_PARAMS(n, A)>        \
      : boost::mpl::and_<                                                       \
            is_callable<T BOOST_PP_ENUM_TRAILING_PARAMS(n, A)>                  \
          , boost::mpl::not_<traits::is_action<                                 \
                typename util::decay<T>::type> >                                \
        >                                                                       \
    {};                                                                         \
    /**/

    BOOST_PP_REPEAT(HPX_PP_ROUND_UP_ADD3(HPX_TUPLE_LIMIT)
      , HPX_TRAITS_DECL_IS_CALLABLE_NOT_ACTION, _);

#   undef HPX_TRAITS_DECL_IS_CALLABLE_NOT_ACTION
}}}

#endif
