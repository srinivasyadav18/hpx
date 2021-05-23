//  Copyright (c) 2021 Srinivas Yadav
//  Copyright (c) 2016 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#if defined(HPX_HAVE_CXX20_EXPERIMENTAL_SIMD)
namespace hpx { namespace execution { inline namespace v1 {
    ///////////////////////////////////////////////////////////////////////////
    struct simd_policy;

    template <typename Executor, typename Parameters>
    struct simd_policy_shim;

    struct simd_task_policy;

    template <typename Executor, typename Parameters>
    struct simd_task_policy_shim;

    ///////////////////////////////////////////////////////////////////////////
    struct simdpar_policy;

    template <typename Executor, typename Parameters>
    struct simdpar_policy_shim;

    struct simdpar_task_policy;

    template <typename Executor, typename Parameters>
    struct simdpar_task_policy_shim;
}}}    // namespace hpx::execution::v1

#endif
