//  Copyright (c) 2021 Srinivas Yadav
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/functional/tag_fallback_dispatch.hpp>

#include <algorithm>
#include <cstddef>
#include <utility>

namespace hpx { namespace parallel { inline namespace v1 { namespace detail {

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iter, typename Sent, typename F>
    constexpr Iter sequential_generate_helper(Iter first, Sent last, F&& f)
    {
        while (first != last)
        {
            *first++ = f();
        }
        return first;
    }

    struct sequential_generate_t
      : hpx::functional::tag_fallback<sequential_generate_t>
    {
    private:
        template <typename ExPolicy, typename Iter, typename Sent, typename F>
        friend constexpr Iter tag_fallback_dispatch(
            hpx::parallel::detail::sequential_generate_t, ExPolicy&&,
            Iter first, Sent last, F&& f)
        {
            return sequential_generate_helper(first, last, std::forward<F>(f));
        }
    };

#if !defined(HPX_COMPUTE_DEVICE_CODE)
    HPX_INLINE_CONSTEXPR_VARIABLE sequential_generate_t sequential_generate =
        sequential_generate_t{};
#else
    template <typename ExPolicy, typename Iter, typename Sent, typename F>
    HPX_HOST_DEVICE HPX_FORCEINLINE Iter sequential_generate(
        ExPolicy&& policy, Iter first, Sent last, F&& f)
    {
        return hpx::parallel::util::sequential_generate_t{}(
            std::forward<ExPolicy>(policy), first, last, std::forward<F>(f));
    }
#endif

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iter, typename F>
    constexpr Iter sequential_generate_n_helper(
        Iter first, std::size_t count, F&& f)
    {
        return std::generate_n(first, count, f);
    }

    struct sequential_generate_n_t
      : hpx::functional::tag_fallback<sequential_generate_n_t>
    {
    private:
        template <typename ExPolicy, typename Iter, typename F>
        friend constexpr Iter tag_fallback_dispatch(
            hpx::parallel::detail::sequential_generate_n_t, ExPolicy&&,
            Iter first, std::size_t count, F&& f)
        {
            return sequential_generate_n_helper(
                first, count, std::forward<F>(f));
        }
    };

#if !defined(HPX_COMPUTE_DEVICE_CODE)
    HPX_INLINE_CONSTEXPR_VARIABLE sequential_generate_n_t
        sequential_generate_n = sequential_generate_n_t{};
#else
    template <typename ExPolicy, typename Iter, typename F>
    HPX_HOST_DEVICE HPX_FORCEINLINE Iter sequential_generate_n(
        ExPolicy&& policy, Iter first, std::size_t count, F&& f)
    {
        return hpx::parallel::util::sequential_generate_n_t{}(
            std::forward<ExPolicy>(policy), first, count, std::forward<F>(f));
    }
#endif

}}}}    // namespace hpx::parallel::v1::detail
