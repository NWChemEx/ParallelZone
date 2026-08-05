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
#include <parallelzone/task/argument_traits.hpp>

namespace parallelzone::task {

/** @brief Class to help forward arguments for generic functions.
 *
 *  @tparam The type of the object *this will wrap.
 *
 *  Depending on how the user passes an argument to a function, the function
 *  will own the value or alias it. ParallelZone is designed to generically
 *  interact with functions and in turn will need to capture arguments. When
 *  PZ captures an argument it will need to prolong the lifetime if the user
 *  passed the argument by value or rvalue reference, and maintain the aliasing
 *  if the user passed the argument by reference.
 *
 *  The ArgumentWrapper class handles the logic of potentially prolonging the
 *  lifetime of the argument, while providing a consistent API regardless of
 *  whether or not it is owning or aliasing the argument.
 */
template<typename T>
class ArgumentWrapper {
private:
    /// Is @p U the same type as *this?
    template<typename U>
    static constexpr bool is_arg_wrapper_v =
      std::is_same_v<ArgumentWrapper<T>, U>;

    /// Disables a template function if @p U is the same type as *this
    template<typename U>
    using disable_if_arg_wrapper = std::enable_if_t<!is_arg_wrapper_v<U>>;

public:
    /// Type traits of @p T needed for TMP
    using traits_t = ArgumentTraits<T>;

    /// Unqualified type of @p T
    using value_type = typename traits_t::value_type;

    /// Type of a mutable reference to an object of type @p value_type
    using reference = typename traits_t::reference;

    /// Type of a read-only reference to an object of type @p value_type
    using const_reference = typename traits_t::const_reference;

    /** @brief Creates a new object which wraps @p object.
     *
     *  @tparam U The type of object must be implicitly convertible to @p T.
     *  @tparam <anonymous> Used to disable this ctor via SFINAE if @p U is
     *                      an object of type ArgumentWrapper.
     *
     *  The value ctor wraps the provided argument, automatically prolonging
     *  its lifetime if needed (needed when @p T is a value or rvalue
     *  reference). If an extended lifetime is not needed, @p object will be
     *  aliased and the user is responsible for ensuring @p object is alive
     *  for the duration of *this.
     *
     *  @param object The value *this is wrapping.
     *
     *  @throw None No throw guarantee.
     */
    template<typename U, typename = disable_if_arg_wrapper<std::decay_t<U>>>
    explicit ArgumentWrapper(U&& object) : m_value_(std::forward<U>(object)) {}

    /** @brief Provides potentially mutable access to the value.
     *
     *  This method is used to access the value. The result will be mutable if
     *  @p T is not const-qualified and read-only if @p T is const-qualified.
     *
     *  @return A reference to the wrapped value.
     *
     *  @throw None No throw guarantee.
     */
    auto& value() { return m_value_; }

    /** @brief Provides read-only access to the value.
     *
     *  This method is identical to the non-const version except that the result
     *  is guarantee to be read-only.
     *
     *  @return A read-only reference to the wrapped value.
     *
     *  @throw None No throw guarantee.
     */
    const auto& value() const { return m_value_; }

private:
    /// Hold by value?
    static constexpr bool hold_by_value =
      traits_t::is_value_v || traits_t::is_rvalue_reference_v;

    /// Hold by reference?
    static constexpr bool hold_by_reference = traits_t::is_reference_v;

    /// Hold by const reference?
    static constexpr bool hold_by_const_reference =
      traits_t::is_const_reference_v;

    /// Type, if being held by reference
    using reference_holder =
      std::conditional_t<hold_by_reference, reference, const_reference>;

    /// Works out the type of the holder
    using holder_type =
      std::conditional_t<hold_by_value, value_type, reference_holder>;

    /// The actual value being held.
    holder_type m_value_;
};

} // namespace parallelzone::task
