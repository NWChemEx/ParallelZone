/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <mpi.h>
#include <parallelzone/mpi_helpers/binary_buffer/binary_buffer.hpp>
#include <parallelzone/mpi_helpers/binary_buffer/binary_view.hpp>
#include <parallelzone/mpi_helpers/traits/gather.hpp>

namespace parallelzone::mpi_helpers {
namespace detail_ {
class CommPPPIMPL;
}

/** @brief A little wrapper around an MPI communicator to make it more C++
 *         friendly.
 *
 *  MPI has a C interface. At one point the standard defined an MPI API, but it
 *  was deprecated (and it didn't do much over the C interface anyways). This
 *  class is meant to provide an object-oriented API to MPI and takes care
 *  of many of the annoying things one may need to do in order to use an
 *  arbitrary C++ object with MPI. Users of parallelzone should consider this
 *  the absolute lowest level API and should not have to ever interact with MPI
 *  directly. The guts of this class are such that they should be able to take
 *  care of every optimization which is possible without the user needing to
 *  break API. That said the underlying implementations are not fully optimized,
 *  so if you see opportunities for optimization please do so under the hood
 *  instead of breaking API.
 */
class CommPP {
public:
    /// Type of the class implementing *this
    using pimpl_type = detail_::CommPPPIMPL;

    /// Type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /// Type of a bare MPI communicator
    using mpi_comm_type = MPI_Comm;

    /// Type used for indexing and offsets
    using size_type = int;

    /// Type of a block of binary data
    using binary_type = BinaryBuffer;

    /// Type of a read/write reference to a block of binary data
    using binary_reference = BinaryView;

    /// Type of a read-only reference to a block of binary data
    using const_binary_reference = ConstBinaryView;

    /// Type returned by the binary version of gather
    using binary_gather_return = gather_return_t<binary_type>;

    /// Type of a buffer and the sizes per rank
    using gatherv_pair = std::pair<binary_type, std::vector<size_type>>;

    /// Type returned by the binary version of gatherv
    using binary_gatherv_return = std::optional<gatherv_pair>;

    // -------------------------------------------------------------------------
    // -- CTors, Assignment, and Dtor
    // -------------------------------------------------------------------------

    /** @brief Creates a null communicator.
     *
     *  The instance resulting from this ctor wraps the MPI_COMM_NULL instance.
     *  The communicator has a size of 0 and the rank of the current process
     *  is MPI_PROC_NULL.
     *
     *  @throw None No throw guarantee.
     */
    CommPP() noexcept;

    /** @brief Creates a communicator which wraps @p comm.
     *
     *  MPI groups processes into communicators. This ctor will initialize *this
     *  so that it wraps the MPI communicator identified by the handle @p comm.
     *
     *  @param[in] comm The handle of the MPI communicator to initialize *this
     *                  from.
     */
    explicit CommPP(mpi_comm_type comm);

    /** @brief Creates a new CommPP instance which is a "deep" copy of @p other.
     *
     *  This ctor initializes *this as a deep copy of @p other.
     */
    CommPP(const CommPP& other);

    ~CommPP() noexcept;

    mpi_comm_type comm() const noexcept;

    /** @brief Returns the number of ranks manged by comm()
     *
     *  In MPI, processes are grouped into communicators. This method is used
     *  to determine how many processes are associated with the communicator
     *  held by *this.
     *
     *  Calling this method is logically the same as calling MPI_Comm_size.
     *  The actual call is only done during construction and the result is
     *  cached in *this.
     *
     *  @return The number of processes associated with the wrapped MPI
     *          communicator.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept;

    /** @brief Returns the rank of the current process.
     *
     *  In MPI processes are grouped into communicators. Each process in the
     *  communicator has a unique ID ranging from 0 to the size of the
     *  communicator.This method is used to retrieve the rank of the current
     *  process.
     *
     *  Calling this method is logically the same as calling MPI_Comm_rank. The
     *  actual call is only done during construction and the result is cached
     *  in *this.
     *
     *  @return The ID of the current process according to the communicator
     *          wrapped by *this.
     *
     *  @throw None No throw guarantee.
     */
    size_type me() const noexcept;

    /** @brief Gathers arbitrary data to MPI process @p root.
     *
     *  In a gather operation involving `N` processes, each process `i` starts
     *  with a piece of data `d_i`. After the call the root process holds `N`
     *  pieces of data such that `i`-th data piece,  is `d_i` which was sent by
     *  process `i`. Only the root process gets data in a traditional gather
     *  call. To give every process a copy of the data one uses an "all gather"
     *  operation.
     *
     *  Users are encouraged to go through this method for all of their gather
     *  needs. This method will auto-magically take care of optimizations using
     *  template meta programming.
     *
     *  @tparam T The qualified type of the data to gather.
     *
     *  @param[in] input This process's contribution to the gather operation.
     *
     *  @param[in] root The rank of the process which will get all of the data.
     *
     *  @return A std::optional which has a value on process @p root an no value
     *          on all other processes. The value depends on @p T. See the
     *          documentation for gather_return_t for more details.
     */
    template<typename T>
    gather_return_t<std::decay_t<T>> gather(T&& input, size_type root) const;

    template<typename T>
    std::decay_t<T> gather(T&& input) const;

    bool operator==(const CommPP& rhs) const noexcept;

    bool operator!=(const CommPP& rhs) const noexcept;

private:
    using opt_root_t = std::optional<size_type>;

    /** @brief Code factorization for the two public templated gather methods.
     *
     * The "_t_" prevents this from getting called recursively when it tries to
     * call  the binary version.
     */
    template<typename T>
    gather_return_t<std::decay_t<T>> gather_t_(T&& input, opt_root_t r) const;

    bool has_pimpl_() const noexcept;

    const pimpl_type& pimpl_() const;

    // -------------------------------------------------------------------------
    // -- Binary-Based MPI Operations
    // -------------------------------------------------------------------------

    /** @brief Binary-based gather call which creates a buffer for the result.
     *
     *  This method is similar to the normal gather call except that it only
     *  works on binary data. This call is most suitable for use when data
     *  needs to be serialized as it will allocate a buffer for the result
     *  which will need to be copied into the final object.
     *
     *  @param[in] data The local bytes to send.
     *
     *  @param[in] root The rank of the process which will get the data.
     *
     *  @return A std::optional which has a value only on the root process. On
     *          the root process this contains the bytes from each process
     *          concatenated together.
     *
     */
    binary_gather_return gather_(const_binary_reference data,
                                 opt_root_t root) const;

    /** @brief Binary-based gather call that can avoid copies.
     *
     *  This call will gather directly into the provided buffer. By making
     *  @p out_buffer alias the byte representation of an object it is possible
     *  to directly gather into an object. This only works if you do not need
     *  to deserialize the object you receive.
     *
     *  @param[in] in_data The local bytes to send.
     *
     *  @param[in] out_buffer A pre-allocated buffer to put the received bytes
     *                        into. Ignored by every process, but the root.
     *  @param[in] root The rank of the root process.
     */
    void gather_(const_binary_reference in_data, binary_reference out_buffer,
                 opt_root_t root) const;

    // binary_gatherv_return gatherv_(const_binary_reference data,
    //                                      size_type root);

    /// The object actually implementing *this
    pimpl_pointer m_pimpl_;
};

} // namespace parallelzone::mpi_helpers

#include "commpp.ipp"
