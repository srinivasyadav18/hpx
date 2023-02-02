//  Copyright (c) 2022 Srinivas Yadav
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/local/execution.hpp>
#include <hpx/local/algorithm.hpp>
#include <hpx/local/init.hpp>
#include <hpx/modules/likwid.hpp>
#include <hpx/modules/testing.hpp>

#include <algorithm>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>
#include <random>

////////////////////////////////////////////////////////////////////////////////
int seed = std::random_device{}();
std::mt19937 gen(seed);

template <typename ExPolicy>
void test_likwid_executor(ExPolicy, const char* name)
{
    std::vector<int> c(10007);
    std::iota(std::begin(c), std::end(c), gen());

    ExPolicy policy;
    auto lik_exe = hpx::execution::likwid_executor(policy.executor(), name);
    auto lik_pol = policy.on(lik_exe);
    hpx::for_each(lik_pol,
        std::begin(c), std::end(c), [](auto t) {return t*t*t;});
}

template <typename ExPolicy>
void test_likwid_executor_async(ExPolicy policy, const char* name)
{
    std::vector<int> c(10007);
    std::iota(std::begin(c), std::end(c), gen());

    auto lik_exe = hpx::execution::likwid_executor(policy.executor(), name);
    auto lik_pol = policy.on(lik_exe);
    auto f = hpx::for_each(lik_pol,
        std::begin(c), std::end(c), [](auto t) {return t*t*t;});

    f.get();
}

void test_for_each()
{
    using namespace hpx::execution;

    test_likwid_executor(seq, "seq_compute");
    // test_likwid_executor(seq, "seq_compute_2");
    test_likwid_executor(par, "par_compute");
    // test_likwid_executor(par, "par_compute_2");
    // test_likwid_executor_async(seq(task), "seq_async_compute");
    // test_likwid_executor_async(seq(task), "seq_async_compute2");
    // test_likwid_executor_async(par(task), "par_async_compute");
}

void test_for_each2()
{
    using namespace hpx::execution;

    // test_likwid_executor(seq, "seq_compute");
    // test_likwid_executor(seq, "seq_compute_2");
    test_likwid_executor(par, "par_compute");
    test_likwid_executor(par, "par_compute_2");
    // test_likwid_executor_async(seq(task), "seq_async_compute");
    // test_likwid_executor_async(seq(task), "seq_async_compute2");
    test_likwid_executor_async(par(task), "par_async_compute");
}

int hpx_main()
{
    std::cout << "using seed: " << seed << std::endl;
    std::srand(seed);
    test_for_each();
    // test_for_each2();
    return hpx::local::finalize();
}

int main(int argc, char* argv[])
{
    // By default this test should run on all available cores
    std::vector<std::string> const cfg = {"hpx.os_threads=all"};

    // Initialize and run HPX
    hpx::local::init_params init_args;
    init_args.cfg = cfg;

    HPX_TEST_EQ_MSG(hpx::local::init(hpx_main, argc, argv, init_args), 0,
        "HPX main exited with non-zero status");

    return hpx::util::report_errors();
}
