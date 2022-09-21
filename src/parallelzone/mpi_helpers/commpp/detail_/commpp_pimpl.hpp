#pragma once
#include <parallelzone/mpi_helpers/commpp/commpp.hpp>

namespace parallelzone::mpi_helpers::detail_ {

/** @brief Basic implementations and state for the CommPP class.
 *
 *  This class primarily exists to facilitate unit testing the implementations
 *  of the MPI ops without exposing them through the public API.
 */
class CommPPPIMPL {
public:
    /// Type of the class we are implementing
    using parent_type = CommPP;

    /// Ultimately a typedef of CommPP::pimpl_pointer
    using pimpl_pointer = parent_type::pimpl_pointer;

    /// Ultimately a typedef of CommPP::mpi_comm_type
    using mpi_comm_type = parent_type::mpi_comm_type;

    /// Ultimately a typedef of CommPP::size_type
    using size_type = parent_type::size_type;

    /// Ultimately a typedef of CommPP::binary_type
    using binary_type = parent_type::binary_type;

    /// Ultimately a typedef of CommPP::binary_reference
    using binary_reference = parent_type::binary_reference;

    /// Ultiamtely a typedef of CommPP::const_binary_reference
    using const_binary_reference = parent_type::const_binary_reference;

    /// Ultimately a typedef of CommPP::binary_gather_return
    using binary_gather_return = parent_type::binary_gather_return;

    /// Ultimately a typedef of CommPP::gatherv_pair
    using gatherv_pair = parent_type::gatherv_pair;

    /// Ultimately a typedef of CommPP::binary_gatherv_return
    using binary_gatherv_return = parent_type::binary_gatherv_return;

    /// Type of an optional root
    using opt_root_t = std::optional<size_type>;

    /** @brief Initializes *this from the MPI communicator @p comm
     *
     *  This ctor inspects @p comm and determines:
     *
     *  - The current MPI rank via MPI_Comm_rank
     *  - The current MPI comm size via MPI_Comm_size
     *
     *  the results are cached in *this.
     *
     *  @param[in] comm Handle to the MPI communicator used for initialization.
     */
    explicit CommPPPIMPL(mpi_comm_type comm);

    /** @brief Makes a deep copy of *this
     *
     *  This method creates a copy of *this by deep copying the wrapped MPI
     *  communicator handle, the rank of the current process, and the size
     *  of the communicator.
     *
     *  @return A deep copy of *this on the heap.
     */
    pimpl_pointer clone() const { return std::make_unique<CommPPPIMPL>(*this); }

    /** @brief Returns the MPI communicator behind *this
     *
     *  When *this was constructed it was given the handle to an MPI
     *  communicator. This method can be used to retrieve that handle.
     *
     *  @return The handle of the MPI communicator *this wraps.
     *
     *  @throw None No throw guarantee.
     */
    mpi_comm_type comm() const noexcept { return m_comm_; }

    /** @brief Returns the number of ranks manged by comm()
     *
     *  In MPI, processes are grouped into communicators. This method is used
     *  to determine how many processes are associated with the communicator
     *  held by *this.
     *
     *  Calling this method is logically the same as calling MPI_Comm_size on
     *  the result of comm(). The actual call to MPI_Comm_size is only done
     *  during construction and the result is cached in *this.
     *
     *  @return The number of processes associated with the wrapped MPI
     *          communicator.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept { return m_size_; }

    /** @brief Returns the rank of the current process.
     *
     *  In MPI processes are grouped into communicators. Each process in the
     *  communicator has a unique integral ID ranging from 0 to the size of the
     *  communicator. This method is used to retrieve the rank of the current
     *  process.
     *
     *  Calling this method is logically the same as calling MPI_Comm_rank on
     *  the result of comm(). The actual call to MPI_Comm_rank is only done
     *  during construction and the result is cached in *this.
     *
     *  @return The ID of the current process according to the communicator
     *          wrapped by *this.
     *
     *  @throw None No throw guarantee.
     */
    size_type me() const noexcept { return m_my_rank_; }

    // -------------------------------------------------------------------------
    // -- MPI Operations
    // -------------------------------------------------------------------------

    /** @brief Binary-based gather call which creates a buffer for the result.
     *
     *  Assume that this communicator has `N` processes and that each process
     *  has a block of binary data that is `b` bytes long. For this call, the
     *  length of the binary data block must be `b` bytes on each process, but
     *  the values in the block can be (and typically are) different. To use
     *  this method, each process provides its local block of binary data and
     *  this method will create a single N * b element binary buffer such that
     *  elements `r * b` through `(r + 1) * b` are the `b` bytes from rank `r`.
     *  If @p root is set, then the output is only local to process @p root,
     *  otherwise each rank of the communicator will have a local copy of the
     *  output.
     *
     *  This is a zero-copy call; however, unless the output data can be
     *  directly used in binary format, it will be necessary to copy out of the
     *  resulting binary buffer to use the results. In some cases, such as when
     *  the object being sent can be interpreted as a contiguous binary buffer,
     *  it will be more efficient to use the other overload of gather, which
     *  allows you to directly gather the result into the target object. See
     *  the documentation for more details.
     *
     *  If @p root is set this method wraps a call to MPI_Gather, if @p root is
     *  not set then this call is analogous to MPI_Allgather. If you can not
     *  guarantee that each process is going to send the same number of bytes
     *  consider using `gatherv`.
     *
     *  @param[in] data The local bytes to send. All processes must send the
     *                  same number of bytes.
     *
     *  @param[in] root The zero-based rank of the process which will get the
     *                  data. If @p root is set this operation will wrap a call
     *                  to MPI_Gather, otherwise (the default scenario), the
     *                  call will be to MPI_Allgather.
     *
     *  @return A std::optional. If @p root is not set, then the std::optional
     *          will contain the gathered bytes on each rank of this
     *          communicator. If @p root is set then the resulting
     *          std::optional will only contain the bytes on the process with
     *          rank @p root.
     *
     */
    binary_gather_return gather(const_binary_reference data,
                                opt_root_t root = std::nullopt) const;

    /** @brief Binary-based gather call that can avoid copies.
     *
     *  This call is identical to the other overload of gather except that it
     *  does not allocate a buffer for the result. Instead the user must
     *  provide a pre-allocated buffer which is minimally of size
     *  `in_data.size() * (this->size())`.
     *
     *  @param[in] data The local bytes we are sending. The length of @p data
     *                  must be the same on each process.
     *
     *  @param[in] out_buffer A pre-allocated buffer to put the bytes into. If
     *                        @p root is set @p out_buffer only needs to be
     *                        allocated on the process with rank @p root,
     *                        otherwise @p out_buffer must be allocated on every
     *                        rank.
     *  @param[in] root The zero-based rank of the root process. This argument
     *                  is optional. If it is set then only @p root will
     *                  receive the result, if it is not set then every
     *                  process gets a copy of the result.
     */
    void gather(const_binary_reference data, binary_reference out_buffer,
                opt_root_t root = std::nullopt) const;

    /** @brief Analog of gather(data, root) where the length of data can vary.
     *
     *  This class's two gather methods must send the same number of bytes from
     *  each process. This method relaxes that restriction. This method will
     *  allocate a buffer for the resulting bytes.
     *
     *  If @p root is not set this method wraps a call to MPI_Allgatherv. If
     *  @p root is set then this wraps a call to MPI_Gatherv.
     *
     *  @param[in] data The local bytes we are sending. The length and content
     *                  can vary from process to process.
     *  @param[in] root The zero-based rank of the process who should get the
     *                  result. If @p root is set only the process with rank
     *                  @p root will get the result, otherwise each process
     *                  gets the result.
     *
     *  @return A std::optional around a pair. The first element of the pair
     *          is the concatenated bytes. The second element is an array such
     *          that the `i`-th element is how many bytes process `i` sent. The
     *          optional has a value on each process if @p root was not set and
     *          only on the process of rank @p root if @p root was set.
     */
    binary_gatherv_return gatherv(const_binary_reference data,
                                  opt_root_t root = std::nullopt) const;

    // -------------------------------------------------------------------------
    // -- Utility functions
    // -------------------------------------------------------------------------

    /** @brief Compares *this to another PIMPL instance.
     *
     *  Two CommPPPIMPL instances are equal if they both wrap the same MPI
     *  communicator. Wrapping the same communicator is sufficient to guarantee
     *  that the communicators also have the same size and that the current
     *  process has the same rank.
     *
     *  The actual handles are compared with MPI_Comm_compare.
     *
     *  @param[in] rhs The PIMPL to compare to *this.
     *
     *  @return True if *this compares equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const CommPPPIMPL& rhs) const noexcept;

private:
    /// The MPI communicator *this wraps
    mpi_comm_type m_comm_;

    /// The MPI rank (on m_comm_) for the current process
    size_type m_my_rank_;

    /// The number of MPI ranks associated with m_comm_
    size_type m_size_;
};

} // namespace parallelzone::mpi_helpers::detail_
