//  Copyright (c) 2021 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// (C) Copyright Jeremy Siek 2002.

#pragma once

#include <hpx/config.hpp>

// we deliberately leave the header guards in place to protect against
// #including this header and the original one from Boost at the same time
#ifndef BOOST_ITERATOR_CATEGORIES_HPP
#define BOOST_ITERATOR_CATEGORIES_HPP

#include <hpx/type_support/identity.hpp>
#include <hpx/type_support/lazy_conditional.hpp>

#include <iterator>
#include <type_traits>

namespace boost {
    namespace iterators {

        // Traversal Categories
        struct no_traversal_tag
        {
        };

        struct incrementable_traversal_tag : no_traversal_tag
        {
        };

        struct single_pass_traversal_tag : incrementable_traversal_tag
        {
        };

        struct forward_traversal_tag : single_pass_traversal_tag
        {
        };

        struct bidirectional_traversal_tag : forward_traversal_tag
        {
        };

        struct random_access_traversal_tag : bidirectional_traversal_tag
        {
        };

        namespace detail {

            // Convert a standard iterator category to a traversal tag.
            // This is broken out into a separate meta-function to reduce
            // the cost of instantiating iterator_category_to_traversal, below,
            // for new-style types.
            template <typename Cat>
            struct std_category_to_traversal
              : hpx::util::lazy_conditional<
                    std::is_convertible<Cat,
                        std::random_access_iterator_tag>::value,
                    hpx::util::identity<random_access_traversal_tag>,
                    hpx::util::lazy_conditional<
                        std::is_convertible<Cat,
                            std::bidirectional_iterator_tag>::value,
                        hpx::util::identity<bidirectional_traversal_tag>,
                        hpx::util::lazy_conditional<
                            std::is_convertible<Cat,
                                std::forward_iterator_tag>::value,
                            hpx::util::identity<forward_traversal_tag>,
                            hpx::util::lazy_conditional<
                                std::is_convertible<Cat,
                                    std::input_iterator_tag>::value,
                                hpx::util::identity<single_pass_traversal_tag>,
                                hpx::util::lazy_conditional<
                                    std::is_convertible<Cat,
                                        std::output_iterator_tag>::value,
                                    hpx::util::identity<
                                        incrementable_traversal_tag>,
                                    hpx::util::identity<no_traversal_tag>>>>>>
            {
            };
        }    // namespace detail

        // Convert an iterator category into a traversal tag
        template <typename Cat>
        struct iterator_category_to_traversal
          : hpx::util::lazy_conditional<
                std::is_convertible<Cat, incrementable_traversal_tag>::value,
                hpx::util::identity<Cat>,
                detail::std_category_to_traversal<Cat>>
        {
        };

        // Trait to get an iterator's traversal category
        template <typename Iterator>
        struct iterator_traversal
          : iterator_category_to_traversal<
                typename std::iterator_traits<Iterator>::iterator_category>
        {
        };

        // Convert an iterator traversal to one of the traversal tags.
        template <typename Traversal>
        struct pure_traversal_tag
          : hpx::util::lazy_conditional<std::is_convertible<Traversal,
                                            random_access_traversal_tag>::value,
                hpx::util::identity<random_access_traversal_tag>,
                hpx::util::lazy_conditional<
                    std::is_convertible<Traversal,
                        bidirectional_traversal_tag>::value,
                    hpx::util::identity<bidirectional_traversal_tag>,
                    hpx::util::lazy_conditional<
                        std::is_convertible<Traversal,
                            forward_traversal_tag>::value,
                        hpx::util::identity<forward_traversal_tag>,
                        hpx::util::lazy_conditional<
                            std::is_convertible<Traversal,
                                single_pass_traversal_tag>::value,
                            hpx::util::identity<single_pass_traversal_tag>,
                            hpx::util::lazy_conditional<
                                std::is_convertible<Traversal,
                                    incrementable_traversal_tag>::value,
                                hpx::util::identity<
                                    incrementable_traversal_tag>,
                                hpx::util::identity<no_traversal_tag>>>>>>
        {
        };

        // Trait to retrieve one of the iterator traversal tags from the
        // iterator category or traversal.
        template <typename Iterator>
        struct pure_iterator_traversal
          : pure_traversal_tag<typename iterator_traversal<Iterator>::type>
        {
        };
    }    // namespace iterators

    using iterators::bidirectional_traversal_tag;
    using iterators::forward_traversal_tag;
    using iterators::incrementable_traversal_tag;
    using iterators::no_traversal_tag;
    using iterators::random_access_traversal_tag;
    using iterators::single_pass_traversal_tag;

    using iterators::iterator_category_to_traversal;
    using iterators::iterator_traversal;
}    // namespace boost

#endif    // BOOST_ITERATOR_CATEGORIES_HPP

namespace hpx {

    using boost::bidirectional_traversal_tag;
    using boost::forward_traversal_tag;
    using boost::incrementable_traversal_tag;
    using boost::no_traversal_tag;
    using boost::random_access_traversal_tag;
    using boost::single_pass_traversal_tag;

    namespace traits {

        ///////////////////////////////////////////////////////////////////////
        template <typename Traversal>
        using pure_traversal_tag =
            boost::iterators::pure_traversal_tag<Traversal>;

        template <typename Traversal>
        using pure_traversal_tag_t =
            typename pure_traversal_tag<Traversal>::type;

        template <typename Traversal>
        HPX_INLINE_CONSTEXPR_VARIABLE bool pure_traversal_tag_v =
            pure_traversal_tag<Traversal>::value;

        template <typename Iterator>
        using pure_iterator_traversal =
            boost::iterators::pure_iterator_traversal<Iterator>;

        template <typename Iterator>
        using pure_iterator_traversal_t =
            typename pure_iterator_traversal<Iterator>::type;

        template <typename Iterator>
        HPX_INLINE_CONSTEXPR_VARIABLE bool pure_iterator_traversal_v =
            pure_iterator_traversal<Iterator>::value;

        ///////////////////////////////////////////////////////////////////////
        template <typename Cat>
        using iterator_category_to_traversal =
            boost::iterators::iterator_category_to_traversal<Cat>;

        template <typename Cat>
        using iterator_category_to_traversal_t =
            typename iterator_category_to_traversal<Cat>::type;

        template <typename Cat>
        HPX_INLINE_CONSTEXPR_VARIABLE bool iterator_category_to_traversal_v =
            iterator_category_to_traversal<Cat>::value;

        template <typename Iterator>
        using iterator_traversal =
            boost::iterators::iterator_traversal<Iterator>;

        template <typename Iterator>
        using iterator_traversal_t =
            typename iterator_traversal<Iterator>::type;

        template <typename Iterator>
        HPX_INLINE_CONSTEXPR_VARIABLE bool iterator_traversal_v =
            iterator_traversal<Iterator>::value;
    }    // namespace traits
}    // namespace hpx
