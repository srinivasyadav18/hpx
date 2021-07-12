//  Copyright (c) 2021 Srinivas Yadav
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>
#include <hpx/functional/invoke.hpp>
#include <hpx/functional/tag_fallback_dispatch.hpp>
#include <hpx/parallel/util/loop.hpp>
#include <hpx/parallel/util/projection_identity.hpp>
#include <hpx/execution/traits/is_execution_policy.hpp>

#include <type_traits>
#include <iostream>
#include <functional>

namespace hpx { namespace parallel { inline namespace v1 { namespace detail {

    template <typename ExPolicy>
    struct sequential_adjacent_find_t : 
      hpx::functional::tag_fallback<sequential_adjacent_find_t<ExPolicy>>
    {
    private:
        template <typename InIter, typename Sent_, typename PredProj>
        friend inline InIter tag_fallback_dispatch(
            hpx::parallel::detail::sequential_adjacent_find_t<ExPolicy>,
            InIter first, Sent_ last, PredProj&& pred_projected)
        {
            std::cout << "tag_fallback_dispatch of adj_diff seq" << std::endl;
            return std::adjacent_find(first, last, 
                        std::forward<PredProj>(pred_projected));
        }

        template <typename ZipIter, typename Token, typename PredProj>
        friend inline void tag_fallback_dispatch(
            hpx::parallel::detail::sequential_adjacent_find_t<ExPolicy>,
            std::size_t base_idx, ZipIter part_begin, std::size_t part_count,
            Token& tok, PredProj&& pred_projected)
        {
            std::cout << "tag_fallback_dispatch of adj_diff par" << std::endl;
            util::loop_idx_n<ExPolicy>(base_idx, part_begin, part_count, tok,
                [&pred_projected, &tok](auto t, std::size_t i) {
                    using hpx::get;
                    if (pred_projected(get<0>(t), get<1>(t)))
                        tok.cancel(i);
                });
        }
    };

#if !defined(HPX_COMPUTE_DEVICE_CODE)
    template <typename ExPolicy>
    HPX_INLINE_CONSTEXPR_VARIABLE sequential_adjacent_find_t<ExPolicy>
        sequential_adjacent_find = sequential_adjacent_find_t<ExPolicy>{};
#else

#endif

}}}}