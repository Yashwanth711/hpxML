//  Copyright (c) 2014 Grant Mercer
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file count.hpp

#if !defined(HPX_PARALLEL_DETAIL_COUNT_JUNE_17_2014_1154AM)
#define HPX_PARALLEL_DETAIL_COUNT_JUNE_17_2014_1154AM

#include <hpx/hpx_fwd.hpp>
#include <hpx/exception_list.hpp>
#include <hpx/parallel/execution_policy.hpp>
#include <hpx/parallel/detail/algorithm_result.hpp>
#include <hpx/parallel/util/partitioner.hpp>
#include <hpx/parallel/util/loop.hpp>
#include <hpx/parallel/util/zip_iterator.hpp>

#include <algorithm>
#include <iterator>

#include <boost/static_assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_base_of.hpp>

namespace hpx { namespace parallel { HPX_INLINE_NAMESPACE(v1)
{
    ///////////////////////////////////////////////////////////////////////////
    // count
    namespace detail
    {
        /// \cond NOINTERNAL
        template <typename ExPolicy, typename InIter, typename T>
        typename detail::algorithm_result<ExPolicy,
            typename std::iterator_traits<InIter>::difference_type>::type
        count(ExPolicy const&, InIter first, InIter last, const T& value,
        boost::mpl::true_)
        {
            typedef typename std::iterator_traits<InIter>::difference_type
                difference;
            try {
                return detail::algorithm_result<ExPolicy, difference>::get(
                    std::count(first, last, value));
            }
            catch (...) {
                detail::handle_exception<ExPolicy>::call();
            }
        }

        template <typename ExPolicy, typename FwdIter, typename T>
        typename detail::algorithm_result<ExPolicy,
            typename std::iterator_traits<FwdIter>::difference_type
        >::type
        count(ExPolicy const& policy, FwdIter first, FwdIter last,
            T const& value, boost::mpl::false_ f)
        {
            typedef typename std::iterator_traits<FwdIter>::value_type type;
            typedef typename std::iterator_traits<FwdIter>::difference_type
                difference_type;

            if (first == last)
            {
                return detail::algorithm_result<ExPolicy, difference_type>
                    ::get( difference_type(0));
            }

            return util::partitioner<ExPolicy, difference_type>::call(
                policy, first, std::distance(first, last),
                [&value](FwdIter part_begin, std::size_t part_count)
                {
                    difference_type ret = std::count(part_begin,
                        std::next(part_begin, part_count), value);
                    return ret;
                },
                hpx::util::unwrapped([](std::vector<difference_type>&& results)
                {
                    return util::accumulate_n(boost::begin(results),
                        boost::size(results), difference_type(0),
                        [](difference_type v1, difference_type v2)
                        {
                            return v1 + v2;
                        });
                }));
        }

        template <typename InIter, typename T>
        typename std::iterator_traits<InIter>::difference_type
        count(execution_policy const& policy, InIter first, InIter last,
            T const& value, boost::mpl::false_)
        {
            HPX_PARALLEL_DISPATCH(policy, detail::count, first, last, value);
        }

        template <typename InIter, typename T>
        typename std::iterator_traits<InIter>::difference_type count(
            execution_policy const& policy, InIter first, InIter last,
            const T& value, boost::mpl::true_ t)
        {
            return detail::count(sequential_execution_policy(),
                first, last, value, t);
        }
        /// \endcond
    }

    /// Returns the number of elements in the range [first, last) satisfying
    /// a specific criteria. This version counts the elements that are equal to
    /// the given \a value.
    ///
    /// \note   Complexity: Performs exactly \a last - \a first comparisons.
    ///
    /// \tparam ExPolicy    The type of the execution policy to use (deduced).
    ///                     It describes the manner in which the execution
    ///                     of the algorithm may be parallelized and the manner
    ///                     in which it executes the comparisons.
    /// \tparam InIter      The type of the source iterators used (deduced).
    ///                     This iterator type must meet the requirements of an
    ///                     input iterator.
    /// \tparam T           The type of the value to search for (deduced).
    ///
    /// \param policy       The execution policy to use for the scheduling of
    ///                     the iterations.
    /// \param first        Refers to the beginning of the sequence of elements
    ///                     the algorithm will be applied to.
    /// \param last         Refers to the end of the sequence of elements the
    ///                     algorithm will be applied to.
    /// \param value        The value to search for.
    ///
    /// The comparisons in the parallel \a count algorithm invoked with
    /// an execution policy object of type \a sequential_execution_policy
    /// execute in sequential order in the calling thread.
    ///
    /// \note The comparisons in the parallel \a count algorithm invoked with
    ///       an execution policy object of type \a parallel_execution_policy or
    ///       \a task_execution_policy are permitted to execute in an unordered
    ///       fashion in unspecified threads, and indeterminately sequenced
    ///       within each thread.
    ///
    /// \returns  The \a count algorithm returns a
    ///           \a hpx::future<difference_type> if the execution policy is of
    ///           type \a task_execution_policy and
    ///           returns \a difference_type otherwise (where \a difference_type
    ///           is defined by \a std::iterator_traits<InIter>::difference_type.
    ///           The \a count algorithm returns the number of elements
    ///           satisfying the given criteria.
    ///
    template <typename ExPolicy, typename InIter, typename T>
    inline typename boost::enable_if<
        is_execution_policy<ExPolicy>,
        typename detail::algorithm_result<ExPolicy,
            typename std::iterator_traits<InIter>::difference_type
        >::type
    >::type
    count(ExPolicy && policy, InIter first, InIter last, T const& value)
    {
        typedef typename std::iterator_traits<InIter>::iterator_category
            category;

        BOOST_STATIC_ASSERT_MSG(
            (boost::is_base_of<std::input_iterator_tag, category>::value),
            "Required at least input iterator .");

        typedef typename boost::mpl::or_<
            is_sequential_execution_policy<ExPolicy>,
            boost::is_same<std::input_iterator_tag, category>
        >::type is_seq;

        return detail::count(std::forward<ExPolicy>(policy),
            first, last, value, is_seq());
    }

    ///////////////////////////////////////////////////////////////////////////
    // count_if
    namespace detail
    {
        /// \cond NOINTERNAL
        template <typename ExPolicy, typename InIter, typename Pred>
        typename detail::algorithm_result<ExPolicy,
            typename std::iterator_traits<InIter>::difference_type
        >::type
        count_if(ExPolicy const&, InIter first, InIter last, Pred && op,
            boost::mpl::true_)
        {
            typedef typename std::iterator_traits<InIter>::difference_type
                difference_type;

            try {
                return detail::algorithm_result<ExPolicy, difference_type>
                    ::get(std::count_if(first, last, std::forward<Pred>(op)));
            }
            catch(...) {
                detail::handle_exception<ExPolicy>::call();
            }
        }

        template <typename ExPolicy, typename FwdIter, typename Pred>
        typename detail::algorithm_result<ExPolicy,
            typename std::iterator_traits<FwdIter>::difference_type
        >::type
        count_if(ExPolicy const& policy, FwdIter first, FwdIter last,
            Pred && op, boost::mpl::false_ f)
        {
            typedef typename std::iterator_traits<FwdIter>::value_type type;
            typedef typename std::iterator_traits<FwdIter>::difference_type
                difference_type;

            if (first == last)
            {
                return detail::algorithm_result<ExPolicy, difference_type>
                    ::get( (difference_type)0);
            }

            return util::partitioner<ExPolicy, difference_type>::call(
                policy, first, std::distance(first, last),
                [op](FwdIter part_begin, std::size_t part_count)
                {
                    difference_type ret = std::count_if(part_begin,
                        std::next(part_begin, part_count), op);
                    return ret;
                },
                hpx::util::unwrapped([](std::vector<difference_type>&& results)
                {
                    return util::accumulate_n(boost::begin(results),
                        boost::size(results), difference_type(0),
                        [](difference_type v1, difference_type v2)
                        {
                            return v1 + v2;
                        });
                }));
        }

        template <typename InIter, typename Pred>
        typename std::iterator_traits<InIter>::difference_type
        count_if(execution_policy const& policy, InIter first, InIter last,
            Pred && op, boost::mpl::false_)
        {
            HPX_PARALLEL_DISPATCH(policy, detail::count_if, first, last,
                std::forward<Pred>(op));
        }

        template <typename InIter, typename Pred>
        typename std::iterator_traits<InIter>::difference_type
        count_if(execution_policy const& policy, InIter first, InIter last,
            Pred && op, boost::mpl::true_ t)
        {
            return detail::count_if(sequential_execution_policy(),
                first, last, std::forward<Pred>(op), t);
        }
        /// \endcond
    }

    /// Returns the number of elements in the range [first, last) satisfying
    /// a specific criteria. This version counts elements for which predicate
    /// \a f returns true.
    ///
    /// \note   Complexity: Performs exactly \a last - \a first applications of
    ///         the predicate.
    ///
    /// \tparam ExPolicy    The type of the execution policy to use (deduced).
    ///                     It describes the manner in which the execution
    ///                     of the algorithm may be parallelized and the manner
    ///                     in which it executes the comparisons.
    /// \tparam InIter      The type of the source iterators used (deduced).
    ///                     This iterator type must meet the requirements of an
    ///                     input iterator.
    /// \tparam F           The type of the function/function object to use
    ///                     (deduced). Unlike its sequential form, the parallel
    ///                     overload of \a count_if requires \a F to meet the
    ///                     requirements of \a CopyConstructible.
    ///
    /// \param policy       The execution policy to use for the scheduling of
    ///                     the iterations.
    /// \param first        Refers to the beginning of the sequence of elements
    ///                     the algorithm will be applied to.
    /// \param last         Refers to the end of the sequence of elements the
    ///                     algorithm will be applied to.
    /// \param f            Specifies the function (or function object) which
    ///                     will be invoked for each of the elements in the
    ///                     sequence specified by [first, last).This is an
    ///                     unary predicate which returns \a true for the
    ///                     required elements. The signature of this predicate
    ///                     should be equivalent to:
    ///                     \code
    ///                     bool pred(const Type &a);
    ///                     \endcode \n
    ///                     The signature does not need to have const&, but
    ///                     the function must not modify the objects passed to
    ///                     it. The type \a Type must be such that an object of
    ///                     type \a InIter can be dereferenced and then
    ///                     implicitly converted to Type.
    ///
    /// \note The assignments in the parallel \a count_if algorithm invoked with
    ///       an execution policy object of type \a sequential_execution_policy
    ///       execute in sequential order in the calling thread.
    /// \note The assignments in the parallel \a count_if algorithm invoked with
    ///       an execution policy object of type \a parallel_execution_policy or
    ///       \a task_execution_policy are permitted to execute in an unordered
    ///       fashion in unspecified threads, and indeterminately sequenced
    ///       within each thread.
    ///
    /// \returns  The \a count_if algorithm returns
    ///           \a hpx::future<difference_type> if the execution policy is of
    ///           type \a task_execution_policy and
    ///           returns \a difference_type otherwise (where \a difference_type
    ///           is defined by \a std::iterator_traits<InIter>::difference_type.
    ///           The \a count algorithm returns the number of elements
    ///           satisfying the given criteria.
    ///
    template <typename ExPolicy, typename InIter, typename F>
    inline typename boost::enable_if<
        is_execution_policy<ExPolicy>,
        typename detail::algorithm_result<ExPolicy,
            typename std::iterator_traits<InIter>::difference_type
        >::type
    >::type
    count_if(ExPolicy && policy, InIter first, InIter last, F && f)
    {
        typedef typename std::iterator_traits<InIter>::iterator_category
            category;

        BOOST_STATIC_ASSERT_MSG(
            (boost::is_base_of<std::input_iterator_tag, category>::value),
            "Required at least input iterator.");

        typedef typename boost::mpl::or_<
            is_sequential_execution_policy<ExPolicy>,
            boost::is_same<std::input_iterator_tag, category>
        >::type is_seq;

        return detail::count_if(std::forward<ExPolicy>(policy),
            first, last, std::forward<F>(f), is_seq());
    }
}}}

#endif
