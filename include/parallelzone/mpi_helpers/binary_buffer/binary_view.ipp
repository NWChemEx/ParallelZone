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
