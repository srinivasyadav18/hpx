//  Copyright (c) 2021 Srinivas Yadav
//  Copyright (c) 2016 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#if defined(HPX_HAVE_CXX20_EXPERIMENTAL_SIMD)

#include <hpx/iterator_support/traits/is_iterator.hpp>
#include <hpx/iterator_support/zip_iterator.hpp>
#include <hpx/type_support/pack.hpp>

#include <hpx/execution/traits/simd_pack_alignment_size.hpp>
#include <hpx/execution/traits/simd_pack_load_store.hpp>
#include <hpx/execution/traits/simd_pack_type.hpp>
#include <hpx/parallel/simd/iterator_helpers.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <numeric>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace parallel { namespace util { namespace detail {
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Iter>
    struct is_data_aligned_impl<hpx::util::zip_iterator<Iter...>>
    {
        template <std::size_t... Is>
        static HPX_FORCEINLINE bool call(
            hpx::util::zip_iterator<Iter...>& it, hpx::util::index_pack<Is...>)
        {
            auto t = it.get_iterator_tuple();
            bool sequencer[] = {true, is_data_aligned(hpx::get<Is>(t))...};
            return std::all_of(&sequencer[1], &sequencer[sizeof...(Is) + 1],
                [](bool val) { return val; });
        }

        static HPX_FORCEINLINE bool call(hpx::util::zip_iterator<Iter...>& it)
        {
            return call(it,
                typename hpx::util::make_index_pack<sizeof...(Iter)>::type());
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename... Iter>
    struct iterator_simd_compatible_impl<hpx::util::zip_iterator<Iter...>>
      : hpx::util::all_of<std::is_arithmetic<
            typename std::iterator_traits<Iter>::value_type>...>
    {
    };
}}}}    // namespace hpx::parallel::util::detail

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace parallel { namespace traits {
    ///////////////////////////////////////////////////////////////////////////
    namespace detail {
        template <typename Tuple, typename... Iter, std::size_t... Is>
        void aligned_pack_load(Tuple& value,
            hpx::util::zip_iterator<Iter...>& iter,
            hpx::util::index_pack<Is...>)
        {
            auto t = iter.get_iterator_tuple();
            int sequencer[] = {0,
                (simd_pack_load<typename hpx::tuple_element<Is, Tuple>::type,
                     typename std::iterator_traits<Iter>::value_type>::
                        aligned(hpx::get<Is>(value), hpx::get<Is>(t)),
                    0)...};
            (void) sequencer;
        }

        template <typename Tuple, typename... Iter, std::size_t... Is>
        void unaligned_pack_load(Tuple& value,
            hpx::util::zip_iterator<Iter...>& iter,
            hpx::util::index_pack<Is...>)
        {
            auto& t = iter.get_iterator_tuple();
            int sequencer[] = {0,
                (simd_pack_load<typename hpx::tuple_element<Is, Tuple>::type,
                     typename std::iterator_traits<Iter>::value_type>::
                        unaligned(hpx::get<Is>(value), hpx::get<Is>(t)),
                    0)...};
            (void) sequencer;
        }
    }    // namespace detail

    template <typename... Vector, typename ValueType>
    struct simd_pack_load<hpx::tuple<Vector...>, ValueType>
    {
        // typedef hpx::tuple<Vector...> value_type;

        template <typename V, typename... Iter>
        static void aligned(V& value, hpx::util::zip_iterator<Iter...>& iter)
        {
            traits::detail::aligned_pack_load(value, iter,
                typename hpx::util::make_index_pack<sizeof...(Iter)>::type());
        }

        template <typename V, typename... Iter>
        static void unaligned(V& value, hpx::util::zip_iterator<Iter...>& iter)
        {
            return traits::detail::unaligned_pack_load(value, iter,
                typename hpx::util::make_index_pack<sizeof...(Iter)>::type());
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    namespace detail {
        template <typename Tuple, typename... Iter, std::size_t... Is>
        void aligned_pack_store(Tuple& value,
            hpx::util::zip_iterator<Iter...>& iter,
            hpx::util::index_pack<Is...>)
        {
            auto t = iter.get_iterator_tuple();
            int sequencer[] = {0,
                (simd_pack_store<typename hpx::tuple_element<Is, Tuple>::type,
                     typename std::iterator_traits<Iter>::value_type>::
                        aligned(hpx::get<Is>(value), hpx::get<Is>(t)),
                    0)...};
            (void) sequencer;
        }

        template <typename Tuple, typename... Iter, std::size_t... Is>
        void unaligned_pack_store(Tuple& value,
            hpx::util::zip_iterator<Iter...>& iter,
            hpx::util::index_pack<Is...>)
        {
            auto& t = iter.get_iterator_tuple();
            int sequencer[] = {0,
                (simd_pack_store<typename hpx::tuple_element<Is, Tuple>::type,
                     typename std::iterator_traits<Iter>::value_type>::
                        unaligned(hpx::get<Is>(value), hpx::get<Is>(t)),
                    0)...};
            (void) sequencer;
        }
    }    // namespace detail

    template <typename... Vector, typename ValueType>
    struct simd_pack_store<hpx::tuple<Vector...>, ValueType>
    {
        template <typename V, typename... Iter>
        static void aligned(V& value, hpx::util::zip_iterator<Iter...>& iter)
        {
            traits::detail::aligned_pack_store(value, iter,
                typename hpx::util::make_index_pack<sizeof...(Iter)>::type());
        }

        template <typename V, typename... Iter>
        static void unaligned(V& value, hpx::util::zip_iterator<Iter...>& iter)
        {
            traits::detail::unaligned_pack_store(value, iter,
                typename hpx::util::make_index_pack<sizeof...(Iter)>::type());
        }
    };
}}}    // namespace hpx::parallel::traits

#endif
