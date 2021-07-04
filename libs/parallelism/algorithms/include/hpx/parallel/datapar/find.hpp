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
#include <hpx/parallel/algorithms/detail/find.hpp>
#include <hpx/parallel/datapar/loop.hpp>
#include <hpx/parallel/util/result_types.hpp>
#include <experimental/simd>

#include <cstddef>
#include <iostream>
#include <type_traits>
#include <utility>

namespace hpx { namespace parallel { inline namespace v1 { namespace detail {

    ///////////////////////////////////////////////////////////////////////////
    template <typename ExPolicy>
    struct datapar_find_if
    {
        template <typename Iterator, typename Sentinel, typename Pred,
            typename Proj>
        static inline constexpr Iterator call(
            Iterator first, Sentinel last, Pred pred, Proj proj)
        {
            std::size_t offset = 0;
            util::cancellation_token<> tok;

            auto ret = util::loop_n<ExPolicy>(first, std::distance(first, last),
                tok, [&offset, &pred, &tok, &proj](auto const& curr) {
                    auto msk =
                        hpx::util::invoke(pred, hpx::util::invoke(proj, *curr));
                    if (std::experimental::any_of(msk))
                    {
                        offset = std::experimental::find_first_set(msk);
                        tok.cancel();
                    }
                });

            if (tok.was_cancelled())
                std::advance(ret, offset);
            return ret;
        }

        template <typename FwdIter, typename Token, typename F, typename Proj>
        static inline constexpr void call(FwdIter part_begin,
            std::size_t part_count, Token& tok, F&& op, Proj&& proj)
        {
            util::loop_n<std::decay_t<ExPolicy>>(part_begin, part_count, tok,
                [&op, &tok, &proj](auto const& curr) {
                    auto msk =
                        hpx::util::invoke(op, hpx::util::invoke(proj, *curr));
                    if (std::experimental::any_of(msk))
                    {
                        tok.cancel();
                    }
                });
        }
    };

    template <typename ExPolicy, typename Iterator, typename Sentinel,
        typename Pred, typename Proj = util::projection_identity>
    inline constexpr std::enable_if_t<
        hpx::is_vectorpack_execution_policy<ExPolicy>::value, Iterator>
    tag_dispatch(hpx::parallel::detail::sequential_find_if_t<ExPolicy>,
        Iterator first, Sentinel last, Pred pred, Proj proj = Proj())
    {
        return datapar_find_if<ExPolicy>::call(first, last, pred, proj);
    }

    template <typename ExPolicy, typename FwdIter, typename Token, typename F,
        typename Proj>
    inline constexpr std::enable_if_t<
        hpx::is_vectorpack_execution_policy<ExPolicy>::value, void>
    tag_dispatch(hpx::parallel::detail::sequential_find_if_t<ExPolicy>,
        FwdIter part_begin, std::size_t part_count, Token& tok, F&& op,
        Proj&& proj)
    {
        return datapar_find_if<ExPolicy>::call(part_begin, part_count, tok,
            std::forward<F>(op), std::forward<Proj>(proj));
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename ExPolicy>
    struct datapar_find_if_not
    {
        template <typename Iterator, typename Sentinel, typename Pred,
            typename Proj>
        static inline constexpr Iterator call(
            Iterator first, Sentinel last, Pred pred, Proj proj)
        {
            std::size_t offset = 0;
            util::cancellation_token<> tok;

            auto ret = util::loop_n<ExPolicy>(first, std::distance(first, last),
                tok, [&offset, &pred, &tok, &proj](auto const& curr) {
                    auto msk = !hpx::util::invoke(
                        pred, hpx::util::invoke(proj, *curr));
                    if (std::experimental::any_of(msk))
                    {
                        offset = std::experimental::find_first_set(msk);
                        tok.cancel();
                    }
                });

            if (tok.was_cancelled())
                std::advance(ret, offset);
            return ret;
        }

        template <typename FwdIter, typename Token, typename F, typename Proj>
        static inline constexpr void call(FwdIter part_begin,
            std::size_t part_count, Token& tok, F&& op, Proj&& proj)
        {
            util::loop_n<std::decay_t<ExPolicy>>(part_begin, part_count, tok,
                [&op, &tok, &proj](auto const& curr) {
                    auto msk =
                        !hpx::util::invoke(op, hpx::util::invoke(proj, *curr));
                    if (std::experimental::any_of(msk))
                    {
                        tok.cancel();
                    }
                });
        }
    };

    template <typename ExPolicy, typename Iterator, typename Sentinel,
        typename Pred, typename Proj = util::projection_identity>
    inline constexpr std::enable_if_t<
        hpx::is_vectorpack_execution_policy<ExPolicy>::value, Iterator>
    tag_dispatch(hpx::parallel::detail::sequential_find_if_not_t<ExPolicy>,
        Iterator first, Sentinel last, Pred pred, Proj proj = Proj())
    {
        return datapar_find_if_not<ExPolicy>::call(first, last, pred, proj);
    }

    template <typename ExPolicy, typename FwdIter, typename Token, typename F,
        typename Proj>
    inline constexpr std::enable_if_t<
        hpx::is_vectorpack_execution_policy<ExPolicy>::value, void>
    tag_dispatch(hpx::parallel::detail::sequential_find_if_not_t<ExPolicy>,
        FwdIter part_begin, std::size_t part_count, Token& tok, F&& op,
        Proj&& proj)
    {
        return datapar_find_if_not<ExPolicy>::call(part_begin, part_count, tok,
            std::forward<F>(op), std::forward<Proj>(proj));
    }
}}}}    // namespace hpx::parallel::v1::detail
#endif
