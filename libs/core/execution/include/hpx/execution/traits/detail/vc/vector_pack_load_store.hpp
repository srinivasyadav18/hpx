//  Copyright (c) 2016 Hartmut Kaiser
//  Copyright (c) 2016 Matthias Kretz
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#if defined(HPX_HAVE_DATAPAR_VC)

#include <cstddef>
#include <iterator>
#include <memory>

#include <Vc/global.h>

#if defined(Vc_IS_VERSION_1) && Vc_IS_VERSION_1

#include <Vc/Vc>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace parallel { namespace traits {
    ///////////////////////////////////////////////////////////////////////////
    template <typename V, typename ValueType, typename Enable>
    struct vector_pack_load
    {
        template <typename Iter>
        HPX_HOST_DEVICE HPX_FORCEINLINE static V aligned(Iter const& iter)
        {
            return V(std::addressof(*iter), Vc::Aligned);
        }

        template <typename Iter>
        HPX_HOST_DEVICE HPX_FORCEINLINE static V unaligned(Iter const& iter)
        {
            return *iter;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename V, typename ValueType, typename Enable>
    struct vector_pack_store
    {
        template <typename Iter>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void aligned(
            V& value, Iter const& iter)
        {
            value.store(std::addressof(*iter), Vc::Aligned);
        }

        template <typename Iter>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void unaligned(
            V& value, Iter const& iter)
        {
            *iter = value;
        }
    };
}}}    // namespace hpx::parallel::traits

#else

#include <Vc/datapar>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace parallel { namespace traits {
    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename Abi, typename NewT>
    struct rebind_pack<Vc::datapar<T, Abi>, NewT>
    {
        typedef Vc::datapar<NewT, Abi> type;
    };

    // don't wrap types twice
    template <typename T, typename Abi1, typename NewT, typename Abi2>
    struct rebind_pack<Vc::datapar<T, Abi1>, Vc::datapar<NewT, Abi2>>
    {
        typedef Vc::datapar<NewT, Abi2> type;
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename V, typename ValueType, typename Enable>
    struct vector_pack_load
    {
        template <typename Iter>
        static typename rebind_pack<V, ValueType>::type aligned(
            Iter const& iter)
        {
            typedef typename rebind_pack<V, ValueType>::type vector_pack_type;
            return vector_pack_type(
                std::addressof(*iter), Vc::flags::vector_aligned);
        }

        template <typename Iter>
        static typename rebind_pack<V, ValueType>::type unaligned(
            Iter const& iter)
        {
            typedef typename rebind_pack<V, ValueType>::type vector_pack_type;
            return vector_pack_type(
                std::addressof(*iter), Vc::flags::element_aligned);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename V, typename ValueType, typename Enable>
    struct vector_pack_store
    {
        template <typename Iter>
        static void aligned(V const& value, Iter const& iter)
        {
            value.copy_to(std::addressof(*iter), Vc::flags::vector_aligned);
        }

        template <typename Iter>
        static void unaligned(V const& value, Iter const& iter)
        {
            value.copy_to(std::addressof(*iter), Vc::flags::element_aligned);
        }
    };
}}}    // namespace hpx::parallel::traits

#endif    // Vc_IS_VERSION_1

#endif
