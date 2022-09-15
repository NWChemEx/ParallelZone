#pragma once
#include <parallelzone/hardware/ram.hpp>
#include <parallelzone/mpi_helpers/commpp.hpp>

namespace parallelzone::hardware::detail_ {

struct RAMPIMPL {
    /// Type of the class *this implements
    using parent_type = RAM;

    /// Ultimately a typedef of RAM::size_type
    using size_type = parent_type::size_type;

    /// Ultimately a typedef of RAM::pimpl_pointer
    using pimpl_pointer = parent_type::pimpl_pointer;

    /// Type of the communicator in this PIMPL
    using comm_type = mpi_helpers::CommPP;

    /** @brief Makes a new PIMPL given the size of the managed RAM, the rank
     *         who owns the RAM, and the MPI communicator.
     *
     */
    RAMPIMPL(size_type size, size_type my_rank, comm_type comm);

    pimpl_pointer clone() const { return std::make_unique<RAMPIMPL>(*this); }

    /// Total size of the RAM
    size_type m_size;

    /// The MPI rank this RAM is local to
    size_type m_rank;

    /// The MPI communicator to communicate with this RAM
    comm_type m_mpi_comm;
};

inline RAMPIMPL::RAMPIMPL(size_type size, size_type rank, comm_type comm) :
  m_size(size), m_rank(rank), m_mpi_comm(comm) {}

} // namespace parallelzone::hardware::detail_
