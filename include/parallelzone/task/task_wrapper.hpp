#pragma once
#include <any>
#include <parallelzone/task/detail_/task_wrapper_.hpp>

namespace parallelzone::task {

/** @brief Infrastructure for type-erasing a task.
 *
 *  A lot of ParallelZone revolves around scheduling, running, and measuring
 *  "tasks". To PZ, a task is nothing more than a call to a callable. The
 *  callable may or may not return something and it may have side-effects (e.g.,
 *  logging). The TaskWrapper class provides a generic type-erased interface for
 *  interacting with the task.
 *
 *  @code
 *  // Say you want to call function foo with arguments arg0, arg1. Then the
 *  // usage is:
 *  auto [task, unwrapper] = make_task(foo, arg0, arg1);
 *
 *  // Run the task
 *  auto result = task();
 *
 *  // Get the result
 *  auto unwrapped_result = unwrapper(result);
 *  @endcode
 *
 *  It should be noted that wrapping functions with zero arguments is also
 *  possible, just pass the name of the function to make_task. You can
 *
 */
class TaskWrapper {
private:
public:
    /// TMP for working out the result of the task
    template<typename FxnType, typename... Args>
    using return_type =
      decltype(std::declval<FxnType>()(std::declval<Args>()...));

    /// Type used to return the results of the task
    using type_erased_return_type = std::any;

    template<typename FxnType, typename... Args>
    TaskWrapper(FxnType&& fxn, Args&&... args) :
      m_erased_function_(detail_::make_outer_lambda_(
        std::forward<FxnType>(fxn), std::forward<Args>(args)...)) {}

    type_erased_return_type operator()() { return m_erased_function_(); }

    /** @brief Creates a function which can unwrap a type-erased result.
     *
     *  The resulting function takes an rvalue reference to the type-erased
     *  result (i.e., you'll probably need to move the result into the function)
     *  and will return the object that was inside the type-erased, not a copy
     *  or reference. In turn you should NOT use the type-erased result again
     *  after calling the function.
     */
    template<typename ReturnType>
    static auto unwrapper() {
        return [](type_erased_return_type&& returned_object) {
            if constexpr(std::is_same_v<ReturnType, void>) {
                return;
            } else {
                return std::any_cast<ReturnType>(std::move(returned_object));
            }
        };
    }

private:
    /// Type *this will use to hold the task
    using erased_function_type = std::function<type_erased_return_type()>;

    /// Type type-erased task
    erased_function_type m_erased_function_;
};

template<typename FxnType, typename... Args>
auto make_task(FxnType&& fxn, Args&&... args) {
    using return_type = TaskWrapper::return_type<FxnType, Args...>;
    auto unwrapper    = TaskWrapper::unwrapper<return_type>();
    TaskWrapper t(std::forward<FxnType>(fxn), std::forward<Args>(args)...);
    return std::make_pair(std::move(t), std::move(unwrapper));
}

} // namespace parallelzone::task