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
#include <parallelzone/task/detail_/task_wrapper_.hpp>

namespace parallelzone::task {

/** @brief Infrastructure for type-erasing a task.
 *
 *  A lot of ParallelZone revolves around scheduling, running, and measuring
 *  "tasks". To PZ, a task is nothing more than executing a callable. The
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
    /// Is type @p U the same as TaskWrapper?
    template<typename U>
    static constexpr bool is_task_wrapper_v = std::is_same_v<U, TaskWrapper>;

    /// Disables a method via SFINAE if @p U is TaskWrapper
    template<typename U>
    using disable_if_task_wrapper_t = std::enable_if_t<!is_task_wrapper_v<U>>;

public:
    /// Type used to return the results of the task
    using type_erased_return_type = std::any;

    /** @brief Value ctor. Creates a TaskWrapper given a callable and arguments.
     *
     *  @tparam FxnType The type of the callable *this will wrap.
     *  @tparam Args The types of the arguments to forward to the wrapped
     *               callable.
     *  @tparam <anonymous> Used to disable this method via SFINAE if
     *                      @p FxnType is TaskWrapper.
     *
     *  Users are encouraged to rely on the `make_task` free function for
     *  creating TaskWrapper objects. That freed function relies on this ctor.
     *  This ctor will create a TensorWrapper object which wraps the provided
     *  callable, @p fxn, and the arguments to pass to @p fxn, @p args. When
     *  *this is executed via `operator()()` @p args will be forwarded to
     *  @p fxn and the result returned as an object of type
     *  @p type_erased_return_type.
     *
     *  @param[in] fxn The callable object this will wrap.
     *  @param[in] args The arguments to forward to @p fxn when *this is
     *                  executed.
     *
     *  @throw ??? Function may throw if capturing @p fxn or @p args throws.
     *             This should be very unlikely though as captures are typically
     *             by reference. Strong throw guarantee.
     */
    template<typename FxnType, typename... Args,
             typename = disable_if_task_wrapper_t<std::decay_t<FxnType>>>
    explicit TaskWrapper(FxnType&& fxn, Args&&... args) :
      m_erased_function_(detail_::make_outer_lambda_(
        std::forward<FxnType>(fxn), std::forward<Args>(args)...)) {}

    /** @brief Takes ownership of another task.
     *
     *  This method will transfer the task wrapped by a different TaskWrapper
     *  object into *this. The TaskWrapper that *this takes the object from
     *  will no longer own the task and is in a valid, but otherwise unspecified
     *  state.
     *
     *  @param[in,out] other The object to take the task from. After this
     *                       operation @p other is in a valid, but otherwise
     *                       unspecified state.
     *
     *  @throw None No throw guarantee.
     */
    TaskWrapper(TaskWrapper&& other) noexcept = default;

    /** @brief Overrides the task in *this with that in @p rhs.
     *
     *  This method will release the task currently wrapped by *this and
     *  replace it with the task wrapped by @p rhs. The task is taken from
     *  @p rhs so that *this owns it and @p rhs no longer does. After this
     *  operation @p rhs is in a valid, but otherwise unspecified state.
     *
     *  @param[in,out] rhs The object to take the task from. After this
     *                     operation @p rhs is in a valid, but otherwise
     *                     unspecified state.
     *
     *  @return *this after overriding the task.
     *
     *  @throw None no throw guarantee.
     */
    TaskWrapper& operator=(TaskWrapper&& rhs) noexcept = default;

    /** @brief Runs the task held by *this.
     *
     *  TaskWrapper objects allow lazy execution the wrapped task. Calling this
     *  method will execute the task. Generally speaking, tasks are single use,
     *  so this function should only be called once. The TaskWrapper class does
     *  NOT enforce this because without knowledge of the wrapped function's
     *  implementation it can not know for sure whether this is the case or
     *  not.
     *
     *  @return A type erased return. If the wrapped function does not return,
     *          the resulting object should NOT be unwrapped.
     *
     *  @throw ??? Throws if the wrapped function throws. Same throw guarantee.
     */
    type_erased_return_type operator()() { return m_erased_function_(); }

    /** @brief Creates a function which can unwrap a type-erased result.
     *
     *  @tparam ReturnType The type to unwrap the object as.
     *
     *  This function creates a callable that takes an rvalue reference to a
     *  @p type_erased_return_type object and returns the object that was
     *  inside the object. Note that the return from the callable is the object,
     *  not a copy or reference. In turn the object passed to the callable is
     *  consumed and should NOT be used again.
     *
     *  @return A callable capable of unwrapping a @p type_erased_return_type
     *          object into a @p ReturnType object.
     *
     *  @throw None No throw guarantee.
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
    /// Deleted because there can only be one instance of a task.
    TaskWrapper(const TaskWrapper&)            = delete;
    TaskWrapper& operator=(const TaskWrapper&) = delete;

    /// Type *this will use to hold the task
    using erased_function_type = std::function<type_erased_return_type()>;

    /// Type type-erased task
    erased_function_type m_erased_function_;
};

/** @brief Wraps a task in a TaskWrapper object.
 *
 *  @related TaskWrapper
 *
 *  @tparam FxnType The type of the callable that *this will wrap.
 *  @tparam Args The types of the arguments that will be forwarded to the
 *               callable.
 *
 *  To interact generically with a task, ParallelZone requires the task to be
 *  wrapped in a TaskWrapper object. When a TaskWrapper object executes its
 *  task the results of the task, if any, are type-erased too. To restore
 *  their types TaskWrapper objects can generate unwrapping callables. While
 *  TaskWrapper objects and unwrapping callables can be made directly through
 *  the TaskWrapper API, the resulting calls are template-heavy. This free
 *  function relies on a little bit of under-the-hood template meta-programming
 *  to take care of the templating for the caller, thus providing a nicer API.
 *
 *  @param[in] fxn The callable to wrap.
 *  @param[in] args The arguments to forward to @p fxn.
 *
 *  @return The task and a callable for unwrapping the result of the task.
 *
 *  @throw ??? This function may throw if capturing the arguments throws. This
 *             should be a very unlikely occurrence. Strong throw guarantee.
 */
template<typename FxnType, typename... Args>
auto make_task(FxnType&& fxn, Args&&... args) {
    using return_type =
      decltype(std::declval<FxnType>()(std::declval<Args>()...));
    auto unwrapper = TaskWrapper::unwrapper<return_type>();
    TaskWrapper t(std::forward<FxnType>(fxn), std::forward<Args>(args)...);
    return std::make_pair(std::move(t), std::move(unwrapper));
}

} // namespace parallelzone::task