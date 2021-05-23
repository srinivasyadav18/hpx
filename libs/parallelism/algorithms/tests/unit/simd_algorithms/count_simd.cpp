//  Copyright (c) 2014 Grant Mercer
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/include/simd.hpp>

#include <iostream>
#include <string>
#include <vector>

#include "../algorithms/count_tests.hpp"

////////////////////////////////////////////////////////////////////////////
template <typename IteratorTag>
void test_count()
{
    using namespace hpx::execution;
    test_count(simd, IteratorTag());
    test_count(simdpar, IteratorTag());

    test_count_async(simd(task), IteratorTag());
    test_count_async(simdpar(task), IteratorTag());
}

void count_test()
{
    test_count<std::random_access_iterator_tag>();
    test_count<std::forward_iterator_tag>();
}

//////////////////////////////////////////////////////////////////////////////
template <typename IteratorTag>
void test_count_exception()
{
    using namespace hpx::execution;

    test_count_exception(simd, IteratorTag());
    test_count_exception(simdpar, IteratorTag());

    test_count_exception_async(simd(task), IteratorTag());
    test_count_exception_async(simdpar(task), IteratorTag());
}

void count_exception_test()
{
    test_count_exception<std::random_access_iterator_tag>();
    test_count_exception<std::forward_iterator_tag>();
}

//////////////////////////////////////////////////////////////////////////////
template <typename IteratorTag>
void test_count_bad_alloc()
{
    using namespace hpx::execution;

    test_count_bad_alloc(simd, IteratorTag());
    test_count_bad_alloc(simdpar, IteratorTag());

    test_count_bad_alloc_async(simd(task), IteratorTag());
    test_count_bad_alloc_async(simdpar(task), IteratorTag());
}

void count_bad_alloc_test()
{
    test_count_bad_alloc<std::random_access_iterator_tag>();
    test_count_bad_alloc<std::forward_iterator_tag>();
}

int hpx_main(hpx::program_options::variables_map& vm)
{
    unsigned int seed = (unsigned int) std::time(nullptr);
    if (vm.count("seed"))
        seed = vm["seed"].as<unsigned int>();

    std::cout << "using seed: " << seed << std::endl;
    std::srand(seed);

    count_test();
    count_exception_test();
    count_bad_alloc_test();
    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    // add command line option which controls the random number generator seed
    using namespace hpx::program_options;
    options_description desc_commandline(
        "Usage: " HPX_APPLICATION_STRING " [options]");

    desc_commandline.add_options()("seed,s", value<unsigned int>(),
        "the random number generator seed to use for this run");

    // By default this test should run on all available cores
    std::vector<std::string> const cfg = {"hpx.os_threads=all"};

    // Initialize and run HPX
    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;
    init_args.cfg = cfg;

    HPX_TEST_EQ_MSG(hpx::init(argc, argv, init_args), 0,
        "HPX main exited with non-zero status");

    return hpx::util::report_errors();
}
