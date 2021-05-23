//  Copyright (c) 2021 Srinivas Yadav
//  Copyright (c) 2016-2017 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#if defined(HPX_HAVE_CXX20_EXPERIMENTAL_SIMD)
#include <hpx/datastructures/tuple.hpp>
#include <hpx/type_support/pack.hpp>

#include <cstddef>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace parallel { namespace traits {
    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename Enable = void>
    struct is_simd_pack : std::false_type
    {
    };

    template <typename T, typename Enable = void>
    struct is_scalar_simd_pack;

    template <typename T, typename Enable>
    struct is_scalar_simd_pack : std::false_type
    {
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename Enable = void>
    struct simd_pack_alignment;

    template <typename... simd>
    struct simd_pack_alignment<hpx::tuple<simd...>,
        typename std::enable_if<
            hpx::util::all_of<is_simd_pack<simd>...>::value>::type>
    {
        typedef
            typename hpx::tuple_element<0, hpx::tuple<simd...>>::type pack_type;

        static std::size_t const value = simd_pack_alignment<pack_type>::value;
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename Enable = void>
    struct simd_pack_size;

    template <typename... simd>
    struct simd_pack_size<hpx::tuple<simd...>,
        typename std::enable_if<
            hpx::util::all_of<is_simd_pack<simd>...>::value>::type>
    {
        typedef
            typename hpx::tuple_element<0, hpx::tuple<simd...>>::type pack_type;

        static std::size_t const value = simd_pack_size<pack_type>::value;
    };
}}}    // namespace hpx::parallel::traits

#if !defined(__CUDACC__)
#include <hpx/execution/traits/detail/simd/simd_pack_alignment_size.hpp>
#endif

#endif