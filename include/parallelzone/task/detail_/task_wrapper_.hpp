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
static auto wrap_args_(Args&&... args) {
    return std::tuple{ArgumentWrapper<Args>(std::forward<Args>(args))...};
}

/// Implements apply_ by expanding tuple. See other overload
template<typename FxnType, typename... Args, std::size_t... I>
static auto apply_(FxnType&& fxn, std::tuple<ArgumentWrapper<Args>...> args,
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
static auto apply_(FxnType&& fxn, std::tuple<ArgumentWrapper<Args>...> args) {
    return apply_(std::forward<FxnType>(fxn), std::move(args),
                  std::make_index_sequence<sizeof...(Args)>());
}

template<typename FxnType, typename... Args>
static auto make_inner_lambda_(FxnType&& fxn, Args&&... args) {
    return [fxn  = std::forward<FxnType>(fxn),
            args = wrap_args_(std::forward<Args>(args)...)]() {
        apply_(fxn, std::move(args));
    };
}

template<typename FxnType, typename... Args>
static auto make_outer_lambda_(FxnType&& fxn, Args&&... args) {
    auto inner_lambda = make_inner_lambda_(std::forward<FxnType>(fxn),
                                           std::forward<Args>(args)...);

    using result_type = decltype(inner_lambda());
    if constexpr(std::is_same_v<result_type, void>) {
        return [inner_lambda = std::move(inner_lambda)]() {
            inner_lambda();
            return std::any{};
        };
    } else {
        return [inner_lambda = std::move(inner_lambda)]() {
            return std::make_any<result_type>(inner_lambda());
        };
    }
}

} // namespace parallelzone::task::detail_