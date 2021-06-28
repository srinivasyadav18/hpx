//  Copyright (c) 2020 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/parallel/util/loop.hpp>

namespace hpx { namespace parallel { inline namespace v1 { namespace detail {

    template <typename ExPolicy, typename Iter, typename Sent, typename T>
    constexpr Iter sequential_fill(
        ExPolicy&& policy, Iter first, Sent last, T const& val)
    {
        hpx::parallel::util::loop_ind(std::forward<ExPolicy>(policy), first,
            last, [&val](auto& v) { v = val; });
        return first;
    }

    template <typename ExPolicy, typename InIter, typename T>
    InIter sequential_fill_n(
        ExPolicy, InIter first, std::size_t count, T const& val)
    {
        hpx::parallel::util::detail::loop_n_ind<ExPolicy>(
            first, count, [&val](auto& v) { v = val; });
        return first;
    }
}}}}    // namespace hpx::parallel::v1::detail
