/*
 * Copyright 2022 NWChemEx-Project
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
#include <type_traits>
#include <vector>

namespace parallelzone::mpi_helpers {

/** @brief Determines if @p T needs to be serialized for MPI communication
 *
 *  This is the primary template. When this template is picked NeedsSerialized
 *  will contain a static member `value` set to true. Users should specialize
 *  this class for types that do not need serialized.
 *
 *  @tparam T The type we are inspecting.
 */
template<typename T, typename = void>
struct NeedsSerialized : std::true_type {};

/// Registers std::string as not needing to be serialized
template<>
struct NeedsSerialized<std::string> : std::false_type {};

/** @brief Specialization for std::vector<T> (@p T trivially copyable)
 *
 *  Trivially copyable types are stored contiguously, thus a std::vector of
 *  them does not need to be serialized (it's perhaps worth noting that
 *  std::vector itself is not trivially copyable).
 *
 */
template<typename T>
struct NeedsSerialized<std::vector<T>,
                       std::enable_if_t<std::is_trivially_copyable_v<T>>>
  : std::false_type {};

/** @brief Convenience value for accessing NeedsSerialized<T>::value
 *
 *  Instead of type `NeedsSerialized<T>::value`, to get whether or not a type
 *  @p T needs to be serialized, users are encouraged to instead use this
 *  convenience value.
 *
 *  @tparam T The type being inspected.
 *
 */
template<typename T>
static constexpr auto needs_serialized_v = NeedsSerialized<T>::value;

} // namespace parallelzone::mpi_helpers
