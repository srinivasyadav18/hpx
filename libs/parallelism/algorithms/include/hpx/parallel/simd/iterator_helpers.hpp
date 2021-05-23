//  Copyright (c) 2021 Srinivas Yadav
//  Copyright (c) 2007-2021 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#if defined(HPX_HAVE_CXX20_EXPERIMENTAL_SIMD)
#include <hpx/assert.hpp>
#include <hpx/execution/traits/simd_pack_alignment_size.hpp>
#include <hpx/execution/traits/simd_pack_load_store.hpp>
#include <hpx/execution/traits/simd_pack_type.hpp>
#include <hpx/functional/detail/invoke.hpp>
#include <hpx/functional/invoke_result.hpp>
#include <hpx/iterator_support/traits/is_iterator.hpp>

#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace parallel { namespace util { namespace detail {
    ///////////////////////////////////////////////////////////////////////////
    template <typename Iter>
    struct is_data_aligned_impl
    {
        static HPX_FORCEINLINE bool call(Iter& it)
        {
            typedef typename std::iterator_traits<Iter>::value_type value_type;
            return (reinterpret_cast<std::uintptr_t>(std::addressof(*it)) &
                       (traits::simd_pack_alignment<value_type>::value - 1)) ==
                0;
        }
    };

    template <typename Iter>
    HPX_FORCEINLINE bool is_data_aligned(Iter& it)
    {
        return is_data_aligned_impl<Iter>::call(it);
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iter1, typename Iter2>
    struct iterators_simd_compatible_impl
    {
        typedef typename std::decay<Iter1>::type iterator1_type;
        typedef typename std::decay<Iter2>::type iterator2_type;

        typedef typename std::iterator_traits<iterator1_type>::value_type
            value1_type;
        typedef typename std::iterator_traits<iterator2_type>::value_type
            value2_type;

        typedef std::integral_constant<bool,
            traits::simd_pack_size<value1_type>::value ==
                    traits::simd_pack_size<value2_type>::value &&
                traits::simd_pack_alignment<value1_type>::value ==
                    traits::simd_pack_alignment<value2_type>::value>
            type;
    };

    template <typename Iter1, typename Iter2>
    struct iterators_simd_compatible
      : iterators_simd_compatible_impl<Iter1, Iter2>::type
    {
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iter, typename Enable = void>
    struct iterator_simd_compatible_impl
      : std::is_arithmetic<typename std::iterator_traits<Iter>::value_type>
    {
    };

    template <typename Iter, typename Enable = void>
    struct iterator_simd_compatible : std::false_type
    {
    };

    template <typename Iter>
    struct iterator_simd_compatible<Iter,
        typename std::enable_if<
            hpx::traits::is_random_access_iterator<Iter>::value>::type>
      : iterator_simd_compatible_impl<typename std::decay<Iter>::type>::type
    {
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iter, typename Enable = void>
    struct simd_loop_step
    {
        typedef typename std::iterator_traits<Iter>::value_type value_type;

        typedef typename traits::simd_pack_type<value_type, 1>::type V1;
        typedef typename traits::simd_pack_type<value_type>::type V;

        template <typename F>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void call1(F&& f, Iter& it)
        {
            V1 tmp;
            traits::simd_pack_load<V1, value_type>::aligned(tmp, it);
            HPX_INVOKE(f, &tmp);
            traits::simd_pack_store<V1, value_type>::aligned(tmp, it);
            ++it;
        }

        template <typename F>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void callv(F&& f, Iter& it)
        {
            V tmp;
            traits::simd_pack_load<V, value_type>::aligned(tmp, it);
            HPX_INVOKE(f, &tmp);
            traits::simd_pack_store<V, value_type>::aligned(tmp, it);
            std::advance(it, traits::simd_pack_size<V>::value);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename Iter, typename Enable = void>
    struct simd_loop_step_ind
    {
        typedef typename std::iterator_traits<Iter>::value_type value_type;

        typedef typename traits::simd_pack_type<value_type, 1>::type V1;
        typedef typename traits::simd_pack_type<value_type>::type V;

        template <typename F>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void call1(F&& f, Iter& it)
        {
            V1 tmp;
            traits::simd_pack_load<V1, value_type>::aligned(tmp, it);
            HPX_INVOKE(f, tmp);
            traits::simd_pack_store<V1, value_type>::aligned(tmp, it);
            ++it;
        }

        template <typename F>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void callv(F&& f, Iter& it)
        {
            V tmp;
            traits::simd_pack_load<V, value_type>::aligned(tmp, it);
            HPX_INVOKE(f, tmp);
            traits::simd_pack_store<V, value_type>::aligned(tmp, it);
            std::advance(it, traits::simd_pack_size<V>::value);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename V1, typename V2>
    struct invoke_simdized_in2
    {
        template <typename F, typename Iter1, typename Iter2>
        static typename hpx::util::invoke_result<F, V1*, V2*>::type
        call_aligned(F&& f, Iter1& it1, Iter2& it2)
        {
            static_assert(traits::simd_pack_size<V1>::value ==
                    traits::simd_pack_size<V2>::value,
                "the sizes of the simd-packs should be equal");

            typedef
                typename std::iterator_traits<Iter1>::value_type value_type1;
            typedef
                typename std::iterator_traits<Iter2>::value_type value_type2;

            V1 tmp1;
            traits::simd_pack_load<V1, value_type1>::aligned(tmp1, it1);
            V2 tmp2;
            traits::simd_pack_load<V2, value_type2>::aligned(tmp2, it2);

            std::advance(it1, traits::simd_pack_size<V1>::value);
            std::advance(it2, traits::simd_pack_size<V2>::value);

            return HPX_INVOKE(std::forward<F>(f), &tmp1, &tmp2);
        }

        template <typename F, typename Iter1, typename Iter2>
        static typename hpx::util::invoke_result<F, V1*, V2*>::type
        call_unaligned(F&& f, Iter1& it1, Iter2& it2)
        {
            static_assert(traits::simd_pack_size<V1>::value ==
                    traits::simd_pack_size<V2>::value,
                "the sizes of the simd-packs should be equal");

            typedef
                typename std::iterator_traits<Iter1>::value_type value_type1;
            typedef
                typename std::iterator_traits<Iter2>::value_type value_type2;

            V1 tmp1;
            traits::simd_pack_load<V1, value_type1>::unaligned(tmp1, it1);
            V2 tmp2;
            traits::simd_pack_load<V2, value_type2>::unaligned(tmp2, it2);

            std::advance(it1, traits::simd_pack_size<V1>::value);
            std::advance(it2, traits::simd_pack_size<V2>::value);

            return HPX_INVOKE(std::forward<F>(f), &tmp1, &tmp2);
        }
    };

    template <typename Iter1, typename Iter2>
    struct simd_loop_step2
    {
        typedef typename std::iterator_traits<Iter1>::value_type value1_type;
        typedef typename std::iterator_traits<Iter2>::value_type value2_type;

        typedef typename traits::simd_pack_type<value1_type, 1>::type V11;
        typedef typename traits::simd_pack_type<value2_type, 1>::type V12;

        typedef typename traits::simd_pack_type<value1_type>::type V1;
        typedef typename traits::simd_pack_type<value2_type>::type V2;

        template <typename F>
        HPX_HOST_DEVICE HPX_FORCEINLINE static
            typename hpx::util::invoke_result<F, V11*, V12*>::type
            call1(F&& f, Iter1& it1, Iter2& it2)
        {
            return invoke_simdized_in2<V11, V12>::call_aligned(
                std::forward<F>(f), it1, it2);
        }

        template <typename F>
        HPX_HOST_DEVICE HPX_FORCEINLINE static
            typename hpx::util::invoke_result<F, V1*, V2*>::type
            callv(F&& f, Iter1& it1, Iter2& it2)
        {
            if (!is_data_aligned(it1) || !is_data_aligned(it2))
            {
                return invoke_simdized_in2<V1, V2>::call_unaligned(
                    std::forward<F>(f), it1, it2);
            }

            return invoke_simdized_in2<V1, V2>::call_aligned(
                std::forward<F>(f), it1, it2);
        }
    };

    ///////////////////////////////////////////////////////////////////////
    template <typename V>
    struct invoke_simdized_inout1
    {
        template <typename F, typename InIter, typename OutIter>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void call_aligned(
            F&& f, InIter& it, OutIter& dest)
        {
            typedef
                typename std::iterator_traits<InIter>::value_type value_type;

            V tmp;
            traits::simd_pack_load<V, value_type>::aligned(tmp, it);

            auto ret = HPX_INVOKE(f, &tmp);
            traits::simd_pack_store<decltype(ret), value_type>::aligned(
                ret, dest);

            std::advance(it, traits::simd_pack_size<V>::value);
            std::advance(dest, ret.size());
        }

        template <typename F, typename InIter, typename OutIter>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void call_unaligned(
            F&& f, InIter& it, OutIter& dest)
        {
            typedef
                typename std::iterator_traits<InIter>::value_type value_type;

            V tmp;
            traits::simd_pack_load<V, value_type>::unaligned(tmp, it);

            auto ret = HPX_INVOKE(f, &tmp);
            traits::simd_pack_store<decltype(ret), value_type>::unaligned(
                ret, dest);

            std::advance(it, traits::simd_pack_size<V>::value);
            std::advance(dest, ret.size());
        }
    };

    template <typename V>
    struct invoke_simdized_inout1_ind
    {
        template <typename F, typename InIter, typename OutIter>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void call_aligned(
            F&& f, InIter& it, OutIter& dest)
        {
            typedef
                typename std::iterator_traits<InIter>::value_type value_type;

            V tmp;
            traits::simd_pack_load<V, value_type>::aligned(tmp, it);

            auto ret = HPX_INVOKE(f, tmp);
            traits::simd_pack_store<decltype(ret), value_type>::aligned(
                ret, dest);

            std::advance(it, traits::simd_pack_size<V>::value);
            std::advance(dest, ret.size());
        }

        template <typename F, typename InIter, typename OutIter>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void call_unaligned(
            F&& f, InIter& it, OutIter& dest)
        {
            typedef
                typename std::iterator_traits<InIter>::value_type value_type;

            V tmp;
            traits::simd_pack_load<V, value_type>::unaligned(tmp, it);

            auto ret = HPX_INVOKE(f, tmp);
            traits::simd_pack_store<decltype(ret), value_type>::unaligned(
                ret, dest);

            std::advance(it, traits::simd_pack_size<V>::value);
            std::advance(dest, ret.size());
        }
    };

    template <typename V1, typename V2>
    struct invoke_simdized_inout2
    {
        template <typename F, typename InIter1, typename InIter2,
            typename OutIter>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void call_aligned(
            F&& f, InIter1& it1, InIter2& it2, OutIter& dest)
        {
            static_assert(traits::simd_pack_size<V1>::value ==
                    traits::simd_pack_size<V2>::value,
                "the sizes of the simd-packs should be equal");

            typedef
                typename std::iterator_traits<InIter1>::value_type value_type1;
            typedef
                typename std::iterator_traits<InIter2>::value_type value_type2;

            V1 tmp1;
            traits::simd_pack_load<V1, value_type1>::aligned(tmp1, it1);
            V2 tmp2;
            traits::simd_pack_load<V2, value_type2>::aligned(tmp2, it2);

            auto ret = HPX_INVOKE(f, &tmp1, &tmp2);
            traits::simd_pack_store<decltype(ret), value_type1>::aligned(
                ret, dest);

            std::advance(it1, traits::simd_pack_size<V1>::value);
            std::advance(it2, traits::simd_pack_size<V2>::value);
            std::advance(dest, ret.size());
        }

        template <typename F, typename InIter1, typename InIter2,
            typename OutIter>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void call_unaligned(
            F&& f, InIter1& it1, InIter2& it2, OutIter& dest)
        {
            static_assert(traits::simd_pack_size<V1>::value ==
                    traits::simd_pack_size<V2>::value,
                "the sizes of the simd-packs should be equal");

            typedef
                typename std::iterator_traits<InIter1>::value_type value_type1;
            typedef
                typename std::iterator_traits<InIter2>::value_type value_type2;

            V1 tmp1;
            traits::simd_pack_load<V1, value_type1>::unaligned(tmp1, it1);
            V2 tmp2;
            traits::simd_pack_load<V2, value_type2>::unaligned(tmp2, it2);

            auto ret = HPX_INVOKE(f, &tmp1, &tmp2);
            traits::simd_pack_store<decltype(ret), value_type1>::unaligned(
                ret, dest);

            std::advance(it1, traits::simd_pack_size<V1>::value);
            std::advance(it2, traits::simd_pack_size<V2>::value);
            std::advance(dest, ret.size());
        }
    };

    struct simd_transform_loop_step
    {
        template <typename F, typename InIter, typename OutIter>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void call1(
            F&& f, InIter& it, OutIter& dest)
        {
            typedef
                typename std::iterator_traits<InIter>::value_type value_type;

            typedef typename traits::simd_pack_type<value_type, 1>::type V1;

            invoke_simdized_inout1<V1>::call_unaligned(
                std::forward<F>(f), it, dest);
        }

        template <typename F, typename InIter1, typename InIter2,
            typename OutIter>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void call1(
            F&& f, InIter1& it1, InIter2& it2, OutIter& dest)
        {
            typedef
                typename std::iterator_traits<InIter1>::value_type value_type1;
            typedef
                typename std::iterator_traits<InIter2>::value_type value_type2;

            typedef typename traits::simd_pack_type<value_type1, 1>::type V1;
            typedef typename traits::simd_pack_type<value_type2, 1>::type V2;

            invoke_simdized_inout2<V1, V2>::call_unaligned(
                std::forward<F>(f), it1, it2, dest);
        }

        ///////////////////////////////////////////////////////////////////
        template <typename F, typename InIter, typename OutIter>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void callv(
            F&& f, InIter& it, OutIter& dest)
        {
            typedef
                typename std::iterator_traits<InIter>::value_type value_type;

            typedef typename traits::simd_pack_type<value_type>::type V;

            HPX_ASSERT(is_data_aligned(it) && is_data_aligned(dest));
            invoke_simdized_inout1_ind<V>::call_aligned(
                std::forward<F>(f), it, dest);
        }

        template <typename F, typename InIter1, typename InIter2,
            typename OutIter>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void callv(
            F&& f, InIter1& it1, InIter2& it2, OutIter& dest)
        {
            typedef
                typename std::iterator_traits<InIter1>::value_type value1_type;
            typedef
                typename std::iterator_traits<InIter2>::value_type value2_type;

            typedef typename traits::simd_pack_type<value1_type>::type V1;
            typedef typename traits::simd_pack_type<value2_type>::type V2;

            HPX_ASSERT(is_data_aligned(it1) && is_data_aligned(it2) &&
                is_data_aligned(dest));
            invoke_simdized_inout2<V1, V2>::call_aligned(
                std::forward<F>(f), it1, it2, dest);
        }
    };

    struct simd_transform_loop_step_ind
    {
        template <typename F, typename InIter, typename OutIter>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void call1(
            F&& f, InIter& it, OutIter& dest)
        {
            typedef
                typename std::iterator_traits<InIter>::value_type value_type;

            typedef typename traits::simd_pack_type<value_type, 1>::type V1;

            invoke_simdized_inout1_ind<V1>::call_unaligned(
                std::forward<F>(f), it, dest);
        }

        ///////////////////////////////////////////////////////////////////
        template <typename F, typename InIter, typename OutIter>
        HPX_HOST_DEVICE HPX_FORCEINLINE static void callv(
            F&& f, InIter& it, OutIter& dest)
        {
            typedef
                typename std::iterator_traits<InIter>::value_type value_type;

            typedef typename traits::simd_pack_type<value_type>::type V;

            HPX_ASSERT(is_data_aligned(it) && is_data_aligned(dest));
            invoke_simdized_inout1_ind<V>::call_aligned(
                std::forward<F>(f), it, dest);
        }
    };
}}}}    // namespace hpx::parallel::util::detail

#endif