#pragma once
#include "binary_view_pimpl.hpp"

namespace parallelzone::mpi_helpers::detail_ {

/** @brief Implements a BinaryView which has to manage the lifetime of the
 *         binary data.
 *
 *
 *  @tparam T The type holding the binary data. @p T is assumed to have two
 *          methods, `data` and `size`, which respectively return: a pointer to
 *          the first element and the number of elements in an object of type
 *          @p T. @p T is also assumed to have a member type `value_type` which
 *          is a typedef of the elements' type.
 *
 */
template<typename T>
class OwningBinaryViewPIMPL : public BinaryViewPIMPLBase {
public:
    /// Ultimately a typedef of BinaryView::const_byte_pointer
    using BinaryViewPIMPLBase::const_byte_pointer;

    /// Ultimately a typedef of BinaryView::size_type
    using BinaryViewPIMPLBase::size_type;

    explicit OwningBinaryDataPIMPL(T obj) : m_obj_(std::move(obj)) {}

private:
    /// Implements data() by calling m_obj_.data() (and casting to std::byte)
    const_byte_pointer data_() const noexcept override;

    /// Implements size() as m_obj_.size() times (size in bytes)
    size_type size_() const noexcept override;

    T m_obj_;
};

// -----------------------------------------------------------------------------
// -- Inline Implementations
// -----------------------------------------------------------------------------

template<typename T>
typename OwningBinaryViewPIMPL<T>::const_byte_pointer
OwningBinaryViewPIMPL<T>::data_() const noexcept {
    const auto* p = m_obj_.data();
    return reinterpret_cast<const_byte_pointer>(p);
}

template<typename T>
typename OwningBinaryViewPIMPL<T>::size_type OwningBinaryViewPIMPL<T>::size_()
  const noexcept {
    return m_obj_.size() * sizeof(typename T::value_type);
}

} // namespace parallelzone::mpi_helpers::detail_
