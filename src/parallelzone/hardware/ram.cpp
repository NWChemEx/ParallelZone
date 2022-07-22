#include <parallelzone/hardware/ram.hpp>

namespace parallelzone::hardware {
namespace detail_ {

struct RAMPIMPL {
    using parent_type = RAM;

    /// Ultimately a typedef of RAM::size_type
    using size_type = parent_type::size_type;

    /// Ultimately a typedef of RAM::pimpl_pointer
    using pimpl_pointer = parent_type::pimpl_pointer;

    explicit RAMPIMPL(size_type size) : m_size(size) {}

    pimpl_pointer clone() const { return std::make_unique<RAMPIMPL>(*this); }

    /// Total size of the RAM
    size_type m_size = 0;
};

} // namespace detail_

// -----------------------------------------------------------------------------
// -- Ctors, Assignment, Dtor
// -----------------------------------------------------------------------------

RAM::RAM() noexcept = default;

RAM::RAM(size_type total_size) :
  RAM(std::make_unique<pimpl_type>(total_size)) {}

RAM::RAM(pimpl_pointer pimpl) noexcept : m_pimpl_(std::move(pimpl)) {}

RAM::RAM(const RAM& other) :
  RAM(other.has_pimpl_() ? other.m_pimpl_->clone() : nullptr) {}

RAM::RAM(RAM&& other) noexcept = default;

RAM& RAM::operator=(const RAM& rhs) {
    if(this != &rhs) RAM(rhs).swap(*this);
    return *this;
}

RAM& RAM::operator=(RAM&& rhs) noexcept = default;

RAM::~RAM() noexcept = default;

// -----------------------------------------------------------------------------
// -- Getters
// -----------------------------------------------------------------------------

RAM::size_type RAM::total_space() const noexcept {
    return !empty() ? m_pimpl_->m_size : 0;
}

// -----------------------------------------------------------------------------
// -- MPI all-to-one operations
// -----------------------------------------------------------------------------

RAM::gather_return_type<double> RAM::gather(double input) const {
    throw std::runtime_error("NYI");
}

RAM::reduce_return_type<double, double> RAM::reduce(double input,
                                                    double fxn) const {
    throw std::runtime_error("NYI");
}

// -----------------------------------------------------------------------------
// -- Utility methods
// -----------------------------------------------------------------------------

bool RAM::empty() const noexcept {
    if(!has_pimpl_()) return true;
    return m_pimpl_->m_size == 0;
}

void RAM::swap(RAM& other) noexcept { m_pimpl_.swap(other.m_pimpl_); }

bool RAM::operator==(const RAM& rhs) const noexcept {
    // Rule out one empty and one non-empty
    if(empty() != rhs.empty()) return false;

    // If both are empty return early
    if(empty()) return true;

    return m_pimpl_->m_size == rhs.m_pimpl_->m_size;
}

// -----------------------------------------------------------------------------
// -- Private methods
// -----------------------------------------------------------------------------

bool RAM::has_pimpl_() const noexcept { return static_cast<bool>(m_pimpl_); }

} // namespace parallelzone::hardware
