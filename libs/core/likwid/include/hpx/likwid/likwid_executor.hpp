//  Copyright (c) 2022 Srinivas Yadav
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <likwid.h>
#include <hpx/execution/execution.hpp>

namespace hpx { namespace execution {

    ///////////////////////////////////////////////////////////////////////////
    template <typename BaseExecutor>
    class likwid_executor
    {
    private:
        struct on_exit
        {
            explicit on_exit(likwid_executor const& exec)
              : exec_(exec)
            {
                likwid_markerStartRegion(exec_.name_);

                // std::cout << "start : " << exec_.name_ << std::endl;
            }

            ~on_exit()
            {
                likwid_markerStopRegion(exec_.name_);

                // std::cout << "end : " << exec_.name_ << std::endl;
            }

            likwid_executor const& exec_;
        };

        template <typename F>
        struct hook_wrapper
        {
            template <typename... Ts>
            decltype(auto) operator()(Ts&&... ts)
            {
                on_exit _{exec_};
                return hpx::invoke(f_, std::forward<Ts>(ts)...);
            }

            likwid_executor const& exec_;
            F f_;
        };

    public:
        using execution_category = typename 
            hpx::parallel::execution::executor_execution_category<BaseExecutor>::type;
        using executor_parameters_type =
            typename hpx::parallel::execution::executor_parameters_type<BaseExecutor>::type;

        template <typename Executor>
        likwid_executor(
            Executor&& exec, const char* name)
          : exec_(HPX_FORWARD(Executor, exec)), name_(name)
        {
        }

        bool operator==(likwid_executor const& rhs) const noexcept
        {
            return exec_ == rhs.exec_;
        }

        bool operator!=(likwid_executor const& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        likwid_executor const& context() const noexcept
        {
            return *this;
        }

    private:
        // OneWayExecutor interface
        template <typename F, typename... Ts>
        friend decltype(auto) tag_invoke(hpx::parallel::execution::sync_execute_t,
            likwid_executor const& exec, F&& f, Ts&&... ts) 
        {
            return hpx::parallel::execution::sync_execute(exec.exec_,
                hook_wrapper<F>{exec, std::forward<F>(f)},
                std::forward<Ts>(ts)...);
        }

        // TwoWayExecutor interface
        template <typename F, typename... Ts>
        friend decltype(auto) tag_invoke(hpx::parallel::execution::async_execute_t,
            likwid_executor const& exec, F&& f, Ts&&... ts) 
        {
            return hpx::parallel::execution::async_execute(exec.exec_,
                hook_wrapper<F>{exec, std::forward<F>(f)},
                std::forward<Ts>(ts)...);
        }

        template <typename F, typename Future, typename... Ts>
        friend decltype(auto) tag_invoke(hpx::parallel::execution::then_execute_t,
            likwid_executor const& exec, F&& f, Future&& predecessor, Ts&&... ts) 
        {
            return hpx::parallel::execution::then_execute(exec.exec_,
                hook_wrapper<F>{exec, std::forward<F>(f)},
                std::forward<Future>(predecessor), std::forward<Ts>(ts)...);
        }

        // NonBlockingOneWayExecutor (adapted) interface
        template <typename F, typename... Ts>
        friend void tag_invoke(hpx::parallel::execution::post_t,
            likwid_executor const& exec, F&& f, Ts&&... ts) 
        {
            hpx::parallel::execution::post(exec.exec_,
                hook_wrapper<F>{exec, std::forward<F>(f)},
                std::forward<Ts>(ts)...);
        }

        // BulkOneWayExecutor interface
        template <typename F, typename S, typename... Ts>
        friend decltype(auto) tag_invoke(hpx::parallel::execution::bulk_sync_execute_t,
            likwid_executor const& exec, F&& f, S const& shape, Ts&&... ts)
        {
            return hpx::parallel::execution::bulk_sync_execute(exec.exec_,
                hook_wrapper<F>{exec, std::forward<F>(f)}, shape,
                std::forward<Ts>(ts)...);
        }

        // BulkTwoWayExecutor interface
        template <typename F, typename S, typename... Ts>
        friend decltype(auto) tag_invoke(hpx::parallel::execution::bulk_async_execute_t,
            likwid_executor const& exec, F&& f, S const& shape, Ts&&... ts)
        {
            return hpx::parallel::execution::bulk_async_execute(exec.exec_,
                hook_wrapper<F>{exec, std::forward<F>(f)}, shape,
                std::forward<Ts>(ts)...);
        }

        template <typename F, typename S, typename Future, typename... Ts>
        friend decltype(auto) tag_invoke(hpx::parallel::execution::bulk_then_execute_t,
            likwid_executor const& exec, F&& f, S const& shape, Future&& predecessor, Ts&&... ts)
        {
            return hpx::parallel::execution::bulk_then_execute(exec.exec_,
                hook_wrapper<F>{exec, std::forward<F>(f)}, shape,
                std::forward<Future>(predecessor), std::forward<Ts>(ts)...);
        }

        // support all properties exposed by the wrapped executor
        // clang-format off
        template <typename Tag, typename Property,
            HPX_CONCEPT_REQUIRES_(
                hpx::execution::experimental::is_scheduling_property_v<Tag> &&
                hpx::functional::is_tag_invocable_v<Tag, BaseExecutor, Property>
            )>
        // clang-format on
        friend likwid_executor tag_invoke(
            Tag tag, likwid_executor const& exec, Property&& prop)
        {
            return likwid_executor(hpx::functional::tag_invoke(
                tag, exec.exec_, HPX_FORWARD(Property, prop)));
        }

        // clang-format off
        template <typename Tag,
            HPX_CONCEPT_REQUIRES_(
                hpx::execution::experimental::is_scheduling_property_v<Tag> &&
                hpx::functional::is_tag_invocable_v<Tag, BaseExecutor>
            )>
        // clang-format on
        friend decltype(auto) tag_invoke(
            Tag tag, likwid_executor const& exec)
        {
            return hpx::functional::tag_invoke(tag, exec.exec_);
        }

    private:
        BaseExecutor exec_;
        const char* name_;
    };

    template <typename Executor>
    likwid_executor(Executor&& exec, const char* name) 
        -> likwid_executor<std::decay_t<Executor>>;
}}

///////////////////////////////////////////////////////////////////////////////
// simple forwarding implementations of executor traits
namespace hpx { namespace parallel { namespace execution {

    template <typename BaseExecutor>
    struct is_one_way_executor<
        hpx::execution::likwid_executor<BaseExecutor>>
      : is_one_way_executor<std::decay_t<BaseExecutor>>
    {
    };

    template <typename BaseExecutor>
    struct is_never_blocking_one_way_executor<
        hpx::execution::likwid_executor<BaseExecutor>>
      : is_never_blocking_one_way_executor<
            std::decay_t<BaseExecutor>>
    {
    };

    template <typename BaseExecutor>
    struct is_two_way_executor<
        hpx::execution::likwid_executor<BaseExecutor>>
      : is_two_way_executor<std::decay_t<BaseExecutor>>
    {
    };

    template <typename BaseExecutor>
    struct is_bulk_one_way_executor<
        hpx::execution::likwid_executor<BaseExecutor>>
      : is_bulk_one_way_executor<std::decay_t<BaseExecutor>>
    {
    };

    template <typename BaseExecutor>
    struct is_bulk_two_way_executor<
        hpx::execution::likwid_executor<BaseExecutor>>
      : is_bulk_two_way_executor<std::decay_t<BaseExecutor>>
    {
    };
}}}    // namespace hpx::parallel::execution