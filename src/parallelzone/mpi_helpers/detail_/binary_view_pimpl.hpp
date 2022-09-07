#pragma once

#include <parallelzone/mpi_helpers/binary_data.hpp>

namespace parallelzone::mpi_helpers::detail_ {

class BinaryViewPIMPL {
public:
    using parent_type = BinaryView;

    using const_byte_pointer = parent_type::const_byte_pointer;

    using size_type = parent_type::size_type;

    const_byte_type data() const noexcept { return data_(); }

    size_type size() const noexcept { return size_(); }

private:
    virtual const_byte_type data_() const noexcept = 0;

    virtual size_type size_() const noexcept = 0;
};

} // namespace parallelzone::mpi_helpers::detail_
