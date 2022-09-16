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

/** @file binary_view.ipp
 *
 *  This file contains the inline definitions for members of the BinaryViewBase
 *  class. This file is meant only for inclusion from binary_view.hpp
 */

namespace parallelzone::mpi_helpers::detail_ {

template<bool IsConst>
template<typename T>
BinaryViewBase<IsConst>::BinaryViewBase(T* p, size_type n) :
  m_p_(reinterpret_cast<pointer>(p)), m_n_(sizeof(std::decay_t<T>) * n) {
    if(p == nullptr && n > 0)
        throw std::runtime_error("Nullptr should have a size of 0. So you "
                                 "either meant to pass a non-null pointer, or "
                                 "you computed the size wrong...");
}

template<bool IsConst>
bool BinaryViewBase<IsConst>::operator==(
  const BinaryViewBase& rhs) const noexcept {
    if(size() != rhs.size()) return false;
    if(size() == 0) return true;
    return std::equal(begin(), end(), rhs.begin());
}

template<bool IsConst>
bool BinaryViewBase<IsConst>::operator!=(
  const BinaryViewBase& rhs) const noexcept {
    return !(*this == rhs);
}

} // namespace parallelzone::mpi_helpers::detail_
