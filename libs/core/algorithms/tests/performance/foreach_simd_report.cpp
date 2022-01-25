//  Copyright (c) 2022 Srinivas Yadav
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/config.hpp>
#if !defined(HPX_COMPUTE_DEVICE_CODE)

#include <hpx/include/compute.hpp>
#include <hpx/include/datapar.hpp>
#include <hpx/local/algorithm.hpp>
#include <hpx/local/chrono.hpp>
#include <hpx/local/execution.hpp>
#include <hpx/local/init.hpp>
#include <hpx/modules/testing.hpp>

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

using std::cos;
using std::sin;

#if defined(HPX_HAVE_CXX20_EXPERIMENTAL_SIMD)
using std::experimental::cos;
using std::experimental::sin;
#endif

struct gen_float_t
{
    std::mt19937 mersenne_engine{42};
    std::uniform_real_distribution<float> dist_float{1, 1024};
    auto operator()()
    {
        return dist_float(mersenne_engine);
    }
} gen_float{};

// Actual test function object
struct test_t
{
    template <typename T>
    void operator()(T& x)
    {
        for (int i = 0; i < 100; i++)
            x = 5.0f * sin(x) + 6.0f * cos(x);
    }
} test_{};

///////////////////////////////////////////////////////////////////////////////
int hpx_main(hpx::program_options::variables_map& vm)
{
    std::size_t vector_size = vm["vector_size"].as<std::size_t>();
    int test_count = vm["test_count"].as<int>();

    // verify that input is within domain of program
    if (test_count == 0 || test_count < 0)
    {
        std::cerr << "test_count cannot be zero or negative...\n" << std::flush;
        hpx::local::finalize();
        return -1;
    }

    using allocator_type = hpx::compute::host::block_allocator<float>;
    using executor_type = hpx::compute::host::block_executor<>;

    auto numa_domains = hpx::compute::host::numa_domains();
    allocator_type alloc(numa_domains);
    executor_type executor(numa_domains);

    {
        {
            hpx::compute::vector<float, allocator_type> vec(
                vector_size, 0.0, alloc);
            hpx::generate(
                hpx::execution::seq, vec.begin(), vec.end(), gen_float);
            hpx::util::perftests_report(
                "foreach_simd", "seq", test_count, [&]() {
                    hpx::for_each(hpx::execution::seq, std::begin(vec),
                        std::end(vec), test_);
                });
        }

        {
            hpx::compute::vector<float, allocator_type> vec(
                vector_size, 0.0, alloc);
            hpx::generate(hpx::execution::par.on(executor), vec.begin(),
                vec.end(), gen_float);
            hpx::util::perftests_report(
                "foreach_simd", "par", test_count, [&]() {
                    hpx::for_each(hpx::execution::par, std::begin(vec),
                        std::end(vec), test_);
                });
        }

#if defined(HPX_HAVE_CXX20_EXPERIMENTAL_SIMD)
        {
            hpx::compute::vector<float, allocator_type> vec(
                vector_size, 0.0, alloc);
            hpx::generate(
                hpx::execution::seq, vec.begin(), vec.end(), gen_float);
            hpx::util::perftests_report(
                "foreach_simd", "simd", test_count, [&]() {
                    hpx::for_each(hpx::execution::simd, std::begin(vec),
                        std::end(vec), test_);
                });
        }

        {
            hpx::compute::vector<float, allocator_type> vec(
                vector_size, 0.0, alloc);
            hpx::generate(hpx::execution::par.on(executor), vec.begin(),
                vec.end(), gen_float);
            hpx::util::perftests_report(
                "foreach_simd", "par_simd", test_count, [&]() {
                    hpx::for_each(hpx::execution::par_simd, std::begin(vec),
                        std::end(vec), test_);
                });
        }
        hpx::util::perftests_print_times();
    }
#else
        {
            hpx::compute::vector<float, allocator_type> vec(
                vector_size, 0.0, alloc);
            hpx::generate(
                hpx::execution::seq, vec.begin(), vec.end(), gen_float);
            hpx::util::perftests_report(
                "foreach_simd", "simd", test_count, [&]() {
                    hpx::for_each(hpx::execution::seq, std::begin(vec),
                        std::end(vec), test_);
                });
        }

        {
            hpx::compute::vector<float, allocator_type> vec(
                vector_size, 0.0, alloc);
            hpx::generate(hpx::execution::par.on(executor), vec.begin(),
                vec.end(), gen_float);
            hpx::util::perftests_report(
                "foreach_simd", "par_simd", test_count, [&]() {
                    hpx::for_each(hpx::execution::par, std::begin(vec),
                        std::end(vec), test_);
                });
        }
#endif
    return hpx::local::finalize();
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    using namespace hpx::program_options;

    options_description cmdline("usage: " HPX_APPLICATION_STRING " [options]");

    // clang-format off
    cmdline.add_options()
        ("vector_size", value<std::size_t>()->default_value(1<<20),
            "size of vector")
        ("test_count", value<int>()->default_value(10),
            "number of tests to be averaged")
        ;
    // clang-format on

    hpx::local::init_params init_args;
    init_args.desc_cmdline = cmdline;
    init_args.cfg = {"hpx.os_threads=all"};

    return hpx::local::init(hpx_main, argc, argv, init_args);
}

#endif