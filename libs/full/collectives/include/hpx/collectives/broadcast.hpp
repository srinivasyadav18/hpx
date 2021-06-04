//  Copyright (c) 2020-2021 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file broadcast.hpp

#pragma once

#if defined(DOXYGEN)
// clang-format off
namespace hpx { namespace collectives {

    /// Broadcast a value to different call sites
    ///
    /// This function sends a set of values to all call sites operating on
    /// the given base name.
    ///
    /// \param  basename    The base name identifying the broadcast operation
    /// \param  local_result A value to transmit to all
    ///                     participating sites from this call site.
    /// \param  num_sites   The number of participating sites (default: all
    ///                     localities).
    /// \param  generation  The generational counter identifying the sequence
    ///                     number of the broadcast operation performed on the
    ///                     given base name. This is optional and needs to be
    ///                     supplied only if the broadcast operation on the
    ///                     given base name has to be performed more than once.
    /// \param this_site    The sequence number of this invocation (usually
    ///                     the locality id). This value is optional and
    ///                     defaults to whatever hpx::get_locality_id() returns.
    /// \params root_site   The site that is responsible for creating the
    ///                     broadcast support object. This value is optional
    ///                     and defaults to '0' (zero).
    ///
    /// \returns    This function returns a future that will become
    ///             ready once the broadcast operation has been completed.
    ///
    template <typename T>
    hpx::future<void> broadcast_to(char const* basename,
        T&& local_result,
        std::size_t num_sites = std::size_t(-1),
        std::size_t this_site = std::size_t(-1),
        std::size_t generation = std::size_t(-1),
        std::size_t root_site = 0)

    /// Broadcast a value to different call sites
    ///
    /// This function sends a set of values to all call sites operating on
    /// the given base name.
    ///
    /// \param  comm        A communicator object returned from \a create_reducer
    /// \param  local_result A value to transmit to all
    ///                     participating sites from this call site.
    /// \param this_site    The sequence number of this invocation (usually
    ///                     the locality id). This value is optional and
    ///                     defaults to whatever hpx::get_locality_id() returns.
    ///
    /// \returns    This function returns a future that will become
    ///             ready once the broadcast operation has been completed.
    ///
    template <typename T>
    hpx::future<void> broadcast_to(communicator comm,
        T&& local_result,
        std::size_t this_site = std::size_t(-1))

    /// Receive a value that was broadcast to different call sites
    ///
    /// This function sends a set of values to all call sites operating on
    /// the given base name.
    ///
    /// \param  basename    The base name identifying the broadcast operation
    /// \param  generation  The generational counter identifying the sequence
    ///                     number of the broadcast operation performed on the
    ///                     given base name. This is optional and needs to be
    ///                     supplied only if the broadcast operation on the
    ///                     given base name has to be performed more than once.
    /// \param this_site    The sequence number of this invocation (usually
    ///                     the locality id). This value is optional and
    ///                     defaults to whatever hpx::get_locality_id() returns.
    ///
    /// \returns    This function returns a future holding the value that was
    ///             sent to all participating sites. It will become
    ///             ready once the broadcast operation has been completed.
    ///
    template <typename T>
    hpx::future<T> broadcast_from(char const* basename,
        std::size_t this_site = std::size_t(-1),
        std::size_t generation = std::size_t(-1))

    /// Receive a value that was broadcast to different call sites
    ///
    /// This function sends a set of values to all call sites operating on
    /// the given base name.
    ///
    /// \param  comm        A communicator object returned from \a create_reducer
    /// \param this_site    The sequence number of this invocation (usually
    ///                     the locality id). This value is optional and
    ///                     defaults to whatever hpx::get_locality_id() returns.
    ///
    /// \returns    This function returns a future holding the value that was
    ///             sent to all participating sites. It will become
    ///             ready once the broadcast operation has been completed.
    ///
    template <typename T>
    hpx::future<T> broadcast_from(communicator comm,
        std::size_t this_site = std::size_t(-1))
}}    // namespace hpx::collectives

// clang-format on
#else

#include <hpx/config.hpp>

#if !defined(HPX_COMPUTE_DEVICE_CODE)

#include <hpx/assert.hpp>
#include <hpx/async_base/launch_policy.hpp>
#include <hpx/async_distributed/async.hpp>
#include <hpx/async_local/dataflow.hpp>
#include <hpx/collectives/create_communicator.hpp>
#include <hpx/components_base/agas_interface.hpp>
#include <hpx/futures/future.hpp>
#include <hpx/modules/execution_base.hpp>
#include <hpx/naming_base/id_type.hpp>
#include <hpx/type_support/unused.hpp>

#include <cstddef>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>

namespace hpx { namespace traits {

    ///////////////////////////////////////////////////////////////////////////
    // support for broadcast
    namespace communication {
        struct broadcast_tag;
    }    // namespace communication

    template <typename Communicator>
    struct communication_operation<Communicator, communication::broadcast_tag>
      : std::enable_shared_from_this<
            communication_operation<Communicator, communication::broadcast_tag>>
    {
        explicit communication_operation(Communicator& comm)
          : communicator_(comm)
        {
        }

        template <typename Result>
        Result get(std::size_t which)
        {
            using arg_type = typename Result::result_type;
            using mutex_type = typename Communicator::mutex_type;
            using lock_type = std::unique_lock<mutex_type>;

            auto this_ = this->shared_from_this();
            auto on_ready = [this_ = std::move(this_)](
                                shared_future<void>&& f) -> arg_type {
                HPX_UNUSED(this_);
                f.get();    // propagate any exceptions

                auto& communicator = this_->communicator_;

                lock_type l(communicator.mtx_);
                return communicator.template access_data<arg_type>(l, 1)[0];
            };

            lock_type l(communicator_.mtx_);
            util::ignore_while_checking<lock_type> il(&l);

            hpx::future<arg_type> f =
                communicator_.gate_.get_shared_future(l).then(
                    hpx::launch::sync, std::move(on_ready));

            communicator_.gate_.synchronize(1, l);

            if (communicator_.gate_.set(which, std::move(l)))
            {
                l = lock_type(communicator_.mtx_);
                communicator_.invalidate_data(l);
            }

            return f;
        }

        template <typename Result, typename T>
        Result set(std::size_t which, T&& t)
        {
            using arg_type = std::decay_t<T>;
            using mutex_type = typename Communicator::mutex_type;
            using lock_type = std::unique_lock<mutex_type>;

            auto this_ = this->shared_from_this();
            auto on_ready = [this_ = std::move(this_)](
                                shared_future<void>&& f) -> arg_type {
                HPX_UNUSED(this_);
                f.get();    // propagate any exceptions

                auto& communicator = this_->communicator_;

                lock_type l(communicator.mtx_);
                return communicator.template access_data<arg_type>(l, 1)[0];
            };

            lock_type l(communicator_.mtx_);
            util::ignore_while_checking<lock_type> il(&l);

            hpx::future<arg_type> f =
                communicator_.gate_.get_shared_future(l).then(
                    hpx::launch::sync, std::move(on_ready));

            communicator_.gate_.synchronize(1, l);

            auto& data = communicator_.template access_data<arg_type>(l, 1);
            data[0] = std::forward<T>(t);

            if (communicator_.gate_.set(which, std::move(l)))
            {
                l = lock_type(communicator_.mtx_);
                communicator_.invalidate_data(l);
            }

            return f;
        }

        Communicator& communicator_;
    };
}}    // namespace hpx::traits

namespace hpx { namespace collectives {

    template <typename T>
    hpx::future<std::decay_t<T>> broadcast_to(communicator fid,
        T&& local_result, std::size_t this_site = std::size_t(-1))
    {
        if (this_site == std::size_t(-1))
        {
            this_site = static_cast<std::size_t>(agas::get_locality_id());
        }

        using arg_type = std::decay_t<T>;

        auto broadcast_data =
            [this_site](communicator&& c,
                arg_type&& local_result) -> hpx::future<arg_type> {
            using action_type = typename detail::communicator_server::
                template communication_set_action<
                    traits::communication::broadcast_tag, hpx::future<arg_type>,
                    arg_type>;

            // make sure id is kept alive as long as the returned future,
            // explicitly unwrap returned future
            hpx::future<arg_type> result =
                async(action_type(), c, this_site, std::move(local_result));

            traits::detail::get_shared_state(result)->set_on_completed(
                [client = std::move(c)]() { HPX_UNUSED(client); });

            return result;
        };

        return dataflow(hpx::launch::sync, std::move(broadcast_data),
            std::move(fid), std::forward<T>(local_result));
    }

    template <typename T>
    hpx::future<std::decay_t<T>> broadcast_to(char const* basename,
        T&& local_result, std::size_t num_sites = std::size_t(-1),
        std::size_t this_site = std::size_t(-1),
        std::size_t generation = std::size_t(-1))
    {
        return broadcast_to(create_communicator(basename, num_sites, this_site,
                                generation, this_site),
            std::forward<T>(local_result), this_site);
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    hpx::future<T> broadcast_from(
        communicator fid, std::size_t this_site = std::size_t(-1))
    {
        if (this_site == std::size_t(-1))
        {
            this_site = static_cast<std::size_t>(agas::get_locality_id());
        }

        auto broadcast_data_direct = [this_site](
                                         communicator&& c) -> hpx::future<T> {
            using action_type = typename detail::communicator_server::
                template communication_get_action<
                    traits::communication::broadcast_tag, hpx::future<T>>;

            // make sure id is kept alive as long as the returned future,
            // explicitly unwrap returned future
            hpx::future<T> result = async(action_type(), c, this_site);

            traits::detail::get_shared_state(result)->set_on_completed(
                [client = std::move(c)]() { HPX_UNUSED(client); });

            return result;
        };

        return dataflow(hpx::launch::sync, std::move(broadcast_data_direct),
            std::move(fid));
    }

    template <typename T>
    hpx::future<T> broadcast_from(char const* basename,
        std::size_t this_site = std::size_t(-1),
        std::size_t generation = std::size_t(-1), std::size_t root_site = 0)
    {
        HPX_ASSERT(this_site != root_site);
        return broadcast_from<T>(create_communicator(basename, std::size_t(-1),
                                     this_site, generation, root_site),
            this_site);
    }
}}    // namespace hpx::collectives

////////////////////////////////////////////////////////////////////////////////
#define HPX_REGISTER_BROADCAST_DECLARATION(...) /**/

////////////////////////////////////////////////////////////////////////////////
#define HPX_REGISTER_BROADCAST(...)             /**/

#endif    // !HPX_COMPUTE_DEVICE_CODE
#endif    // DOXYGEN
