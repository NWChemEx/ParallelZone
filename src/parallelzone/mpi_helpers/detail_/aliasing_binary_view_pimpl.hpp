#pragma once

#include "binary_view_pimpl.hpp"

namespace parallelzone::mpi_helpers::detail_ {

class AliasingBinaryViewPIMPL : public BinaryViewPIMPLBase {
public:
    /// Ultimately a typedef of BinaryView::const_byte_pointer
    using BinaryViewPIMPLBase::const_byte_pointer;

    /// Ultimately a typedef of BinaryView::size_type
    using BinaryViewPIMPLBase::size_type;

    AliasingBinaryViewPIMPL(const_byte_pointer p, size_type n) :
      m_p_(p), m_n_(n) {}

private:
    /// Implements data() by returning the held pointer
    const_byte_pointer data_() const noexcept override { return m_p_; }

    /// Implements size() by returning the held length
    size_type size_() const noexcept override { return m_n_; }

    /// Pointer to the data
    const_byte_pointer m_p_;

    /// Length of the data
    size_type m_n_;
};

} // namespace parallelzone::mpi_helpers::detail_
