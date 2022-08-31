#include "detail_/ram_pimpl.hpp"
#include <stdexcept>

namespace parallelzone::hardware {

// -----------------------------------------------------------------------------
// -- Ctors, Assignment, Dtor
// -----------------------------------------------------------------------------

RAM::RAM() noexcept = default;

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

void RAM::assert_pimpl_() const {
    if(has_pimpl_) return;
    throw std::runtime_error("The current RAM instance is null. Was it default "
                             "constructed or moved from?");
}

RAM::binary_return_type RAM::gather_(std::byte* data, size_type n) const {
    assert_pimpl_();

    // Get the MPI communicator, the comm's size, and the current rank
    int me, comm_size;
    auto comm = m_pimpl_->m_mpi_comm;
    MPI_Comm_rank(comm, &me);
    MPI_Comm_size(comm, &comm_size);

    // MPI expects integers so convert size_type objects to ints
    int n_int(n);
    int root(m_pimpl_->m_rank);

    // Each rank sends n bytes, so if I'm root I get comm_size * n bytes.
    // All other ranks get nothing
    bool am_i_root = me == root;
    int recv_size  = !am_i_root ? 0 : comm_size * n_int;

    // Allocate buffer, and get a pointer to it
    binary_data buffer(recv_size);
    auto pbuffer = buffer.data();

    MPI_Gather(data, n_int, MPI_BYTE, pbuffer, n_int, MPI_BYTE, root, comm);

    // Prepare and return the result
    binary_return_type rv;
    if(am_i_root) rv.emplace(std::move(buffer));
    return rv;
}

} // namespace parallelzone::hardware
