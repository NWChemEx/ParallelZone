#pragma once

namespace parallelzone::mpi_helpers {
namespace detail_ {
class BinaryViewPIMPL;
}

class BinaryView {
public:
    using byte_type = std::byte;

    using const_byte_pointer = const byte_type*;

    using size_type = std::size_t;

    using pimpl_type = detail_::BinaryViewPIMPL;

    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    BinaryView() noexcept;

    BinaryView(const_byte_pointer p, size_type n);

    BinaryView(std::string s);

    BinaryView(pimpl_pointer p) noexcept;

    const_byte_pointer data() const noexcept;

    size_type size() const noexcept;

private:
    pimpl_pointer m_pimpl_;
};

} // namespace parallelzone::mpi_helpers
