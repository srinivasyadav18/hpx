//  Copyright (c) 2022 Srinivas Yadav
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#if defined(HPX_HAVE_DATAPAR_EXPERIMENTAL_SIMD)

#include <hpx/execution/traits/detail/simd/vector_pack_simd.hpp>

#include <cstddef>

namespace hpx { namespace parallel { namespace traits {
    ////////////////////////////////////////////////////////////////////
    template <typename T, typename Abi>
    HPX_HOST_DEVICE HPX_FORCEINLINE auto choose(
        SIMD_NAMESPACE::simd_mask<T, Abi> const& msk,
        SIMD_NAMESPACE::simd<T, Abi> const& v_true,
        SIMD_NAMESPACE::simd<T, Abi> const& v_false)
    {
#if defined(HPX_HAVE_DATAPAR_SVE)
        return SIMD_NAMESPACE::choose(msk, v_true, v_true);
#endif
        SIMD_NAMESPACE::simd<T, Abi> v;
        where(msk, v) = v_true;
        where(!msk, v) = v_false;
        return v;
    }

    ////////////////////////////////////////////////////////////////////
    template <typename T, typename Abi>
    HPX_HOST_DEVICE HPX_FORCEINLINE void mask_assign(
        SIMD_NAMESPACE::simd_mask<T, Abi> const& msk,
        SIMD_NAMESPACE::simd<T, Abi>& v,
        SIMD_NAMESPACE::simd<T, Abi> const& val)
    {
#if defined(HPX_HAVE_DATAPAR_STD_EXPERIMENTAL_SIMD)
        where(msk, v) = val;
#endif
#if defined(HPX_HAVE_DATAPAR_SVE)
        SIMD_NAMESPACE::mask_assign(msk, v, val);
#endif
    }
}}}    // namespace hpx::parallel::traits

#endif
