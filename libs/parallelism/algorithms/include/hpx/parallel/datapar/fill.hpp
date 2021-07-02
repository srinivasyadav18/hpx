//  Copyright (c) 2021 Srinivas Yadav
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#if defined(HPX_HAVE_DATAPAR)
#include <hpx/execution/traits/is_execution_policy.hpp>
#include <hpx/functional/tag_dispatch.hpp>
#include <hpx/parallel/algorithms/detail/fill.hpp>
#include <hpx/parallel/datapar/transform_loop.hpp>
#include <hpx/parallel/util/result_types.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace hpx { namespace parallel { inline namespace v1 { namespace detail {

    ///////////////////////////////////////////////////////////////////////////
    struct datapar_fill
    {
        template <typename Iter, typename Sent, typename T>
        HPX_HOST_DEVICE HPX_FORCEINLINE static
            typename std::enable_if<iterator_datapar_compatible<Iter>::value,
                Iter>::type
            call(ExPolicy&& Policy, Iter first, Sent last, T const& value)
        {
            hpx::parallel::util::loop_ind(std::forward<ExPolicy>(policy), first,
                last, [&val](auto& v) { v = val; });
            return first;
        }

        template <typename Iter, typename Sent, typename T>
        HPX_HOST_DEVICE HPX_FORCEINLINE static
            typename std::enable_if<!iterator_datapar_compatible<Iter>::value,
                Iter>::type
            call(ExPolicy&&, Iter first, Sent last, T const& value)
        {
            return sequential_fill_helper(first, last, value);
        }
    };

    template <typename ExPolicy, typename Iter, typename Sent, typename T>
    HPX_HOST_DEVICE HPX_FORCEINLINE typename std::enable_if<
        hpx::is_vectorpack_execution_policy<ExPolicy>::value, Iter>::type
    tag_dispatch(hpx::parallel::detail::sequential_fill_t, ExPolicy&& policy,
        Iter first, Sent last, T const& value)
    {
        return datapar_fill::call(
            std::forward<ExPolicy>(policy), first, last, value);
    }

    ///////////////////////////////////////////////////////////////////////////
    struct datapar_fill_n
    {
        template <typename Iter, typename T>
        HPX_HOST_DEVICE HPX_FORCEINLINE static
            typename std::enable_if<iterator_datapar_compatible<Iter>::value,
                Iter>::type
            call(ExPolicy&& Policy, Iter first, std::size_t count,
                T const& value)
        {
            hpx::parallel::util::loop_n_ind(std::forward<ExPolicy>(policy),
                first, count, [&val](auto& v) { v = val; });
            return first;
        }

        template <typename Iter, typename T>
        HPX_HOST_DEVICE HPX_FORCEINLINE static
            typename std::enable_if<!iterator_datapar_compatible<Iter>::value,
                Iter>::type
            call(ExPolicy&&, Iter first, std::size_t count, T const& value)
        {
            return sequential_fill_n_helper(first, count, value);
        }
    };

    template <typename ExPolicy, typename Iter, typename T>
    HPX_HOST_DEVICE HPX_FORCEINLINE typename std::enable_if<
        hpx::is_vectorpack_execution_policy<ExPolicy>::value, Iter>::type
    tag_dispatch(hpx::parallel::detail::sequential_fill_n_t, ExPolicy&& policy,
        Iter first, std::size_t count, T const& value)
    {
        return datapar_fill_n::call(
            std::forward<ExPolicy>(policy), first, count, value);
    }
}}}}    // namespace hpx::parallel::v1::detail
#endif
