/*
 * Copyright 2025 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <any>
#include <parallelzone/task/argument_wrapper.hpp>
#include <tuple>
namespace parallelzone::task::detail_ {

/** @brief Creates a tuple that will forward arguments.
 *
 *  @tparams Args The types of @p args
 *
 *  In order to store arguments we need to put them in ArgumentWrapper objects
 *  (which will manage the lifetime). This function forwards a parameter pack
 *  into a tuple of ArgumentWrapper objects.
 *
 *  @param[in] args The arguments to wrap into a tuple.
 *
 *  @throw None No throw guarantee.
 */
template<typename... Args>
auto wrap_args_(Args&&... args) {
    return std::tuple{ArgumentWrapper<Args>(std::forward<Args>(args))...};
}

/// Implements apply_ by expanding tuple. See other overload
template<typename FxnType, typename... Args, std::size_t... I>
auto apply_(FxnType&& fxn, std::tuple<ArgumentWrapper<Args>...> args,
            std::index_sequence<I...>) {
    return fxn(std::forward<Args>(std::get<I>(args).value())...);
}

/** @brief Calls a callable with the provided arguments.
 *
 *  @tparam FxnType The type of the callable.
 *  @tparam Args The types of the arguments as the user wants them to be passed.
 *
 *  Most functions expect positional arguments and not a tuple of arguments,
 *  but tuples are more convenient for meta-programming. This function will
 *  take a tuple and unpack it into the positional arguments of the provided
 *  function, and then call the function.
 *
 *  @note This function reimplements std::apply because we need to additionally
 *         call `.value()` on each element of the tuple.
 *
 *  @param[in] fxn The callable to call.
 *  @param[in] args A tuple containing the arguments to forward to @p fxn.
 *
 *  @return Returns if @p fxn returns.
 *
 *  @throws Throws if @p fxn throws. Same throw guarantee.
 */
template<typename FxnType, typename... Args>
auto apply_(FxnType&& fxn, std::tuple<ArgumentWrapper<Args>...> args) {
    return apply_(std::forward<FxnType>(fxn), std::move(args),
                  std::make_index_sequence<sizeof...(Args)>());
}

/** @brief Creates a lambda that wil forward @p args to @p fxn.
 *
 *  @tparam FxnType A callable type.
 *  @tparam Args The types of args
 *
 *  This function creates a lambda that can be called with no arguments. When
 *  the lambda is called, @p args will be forwarded to @p fxn and the result
 *  returned from the lambda, i.e., this function creates a lambda that can be
 *  used to run @p fxn lazily.
 *
 *  @param[in] fxn The callable to run.
 *  @param[in] args The arguments to forward to @p fxn.
 *
 *  @return A lambda that wraps the process of forwarding @p args to @p fxn.
 *
 *  @throw None No throw guarantee.
 */
template<typename FxnType, typename... Args>
auto make_inner_lambda_(FxnType&& fxn, Args&&... args) {
    auto wrapped_args = wrap_args_(std::forward<Args>(args)...);
    auto l            = [fxn  = std::forward<FxnType>(fxn),
              args = std::move(wrapped_args)]() mutable {
        return apply_(fxn, std::move(args));
    };
    return l;
}

template<typename FxnType, typename... Args>
static auto make_outer_lambda_(FxnType&& fxn, Args&&... args) {
    auto inner_lambda = make_inner_lambda_(std::forward<FxnType>(fxn),
                                           std::forward<Args>(args)...);

    using result_type = decltype(inner_lambda());
    if constexpr(std::is_same_v<result_type, void>) {
        return [inner_lambda = std::move(inner_lambda)]() mutable {
            inner_lambda();
            return std::any{};
        };
    } else {
        return [inner_lambda = std::move(inner_lambda)]() mutable {
            return std::make_any<result_type>(inner_lambda());
        };
    }
}

} // namespace parallelzone::task::detail_