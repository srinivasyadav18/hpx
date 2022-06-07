//  Copyright (c) 2022 Srinivas Yadav
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#if defined(HPX_HAVE_DATAPAR_EXPERIMENTAL_SIMD)

#if defined(HPX_HAVE_DATAPAR_STD_EXPERIMENTAL_SIMD)
#include <experimental/simd>
namespace SIMD_NAMESPACE = std::experimental;
#endif

#if defined(HPX_HAVE_DATAPAR_SVE)
#include <sve/sve.hpp>
namespace SIMD_NAMESPACE = sve::experimental;
#endif

#endif
