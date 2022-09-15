#pragma once
#include <mpi.h>
#include <parallelzone/mpi_helpers/binary_buffer/binary_buffer.hpp>
#include <parallelzone/mpi_helpers/binary_buffer/binary_view.hpp>

namespace parallelzone::mpi_helpers {

/** @brief A little wrapper around an MPI communicator to make it more C++
 *         friendly.
 *
 */
class CommPP {
public:
    /// Type of a bare MPI communicator
    using mpi_comm_type = MPI_Comm;

    /// MPI uses int, not std::size_t
    using size_type = int;

    using binary_type = BinaryBuffer;

    using binary_reference = BinaryView;

    using const_binary_reference = ConstBinaryView;

    CommPP(mpi_comm_type comm);

    mpi_comm_type comm() const;

    size_type size() const;

    size_type me() const;

    using gather_return = std::optional<binary_type>;

    /** @brief Most user-friendly gather call possible (w/o knowing root).
     *
     *  This gather call wraps the process of creating a buffer on process
     *  @p root and gathering @p data from all processes to @p root. For many
     *  C++ scenarios this is what you want because you can then deserialize
     *  out of the buffer. However, if you do not need to deserialize, using
     *  this method will result in an extra copy. Consider a different overload
     *  of gather if you do not need to deserialize.
     *
     */
    gather_return gather(const_binary_reference data, size_type root) const;

    /** @brief Slightly less user-friendly gather call, but can avoid copies.
     *
     *  This call will gather directly into the provided buffer.
     *
     */
    void gather(const_binary_reference in_data, binary_reference out_buffer,
                size_type root) const;

private:
    mpi_comm_type m_comm_;

    size_type m_my_rank_;

    size_type m_size_;
};

} // namespace parallelzone::mpi_helpers
