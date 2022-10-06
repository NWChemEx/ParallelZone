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
#include "../../hardware/ram/detail_/ram_pimpl.hpp"
#include <parallelzone/mpi_helpers/commpp/commpp.hpp>
#include <parallelzone/runtime/resource_set.hpp>
#include <parallelzone/runtime/runtime_view.hpp>

namespace parallelzone::runtime::detail_ {

/** @brief Implements the ResourceSet.
 *
 *  Design notes:
 *  - Types are primarily pulled in from the ResourceSet class
 */
struct ResourceSetPIMPL {
    /// Type *this implements, typedef of ResourceSet
    using resource_set_type = ResourceSet;

    /// How the RuntimeView accesses MPI, typedef of CommPP
    using mpi_comm_type = mpi_helpers::CommPP;

    /// Type used to model RAM, ultimately typedef of ResourceSet::ram_type
    using ram_type = resource_set_type::ram_type;

    /// Type used for indexing, ultimately typedef of ResourceSet::size_type
    using size_type = resource_set_type::size_type;

    /// Type of pointer ResourceSet used to store *this, ultimately typedef of
    /// ResourceSet::pimpl_pointer
    using pimpl_pointer = resource_set_type::pimpl_pointer;

    /// Type of the loggers. ultimately a typedef of ResourceSet::logger_type
    using logger_type = resource_set_type::logger_type;

    /// Type of a read/write reference to a logger, ultimately a typedef of
    /// ResourceSet::logger_reference
    using logger_reference = resource_set_type::logger_reference;

    /// Type of a view to a logger instance
    using logger_pointer = std::shared_ptr<logger_type>;

    /** @brief Initializes *this with the resources owned by process @p rank on
     *          MPI communicator @p my_mpi.
     *
     *  @param[in] rank The current process's rank on @p my_mpi
     *  @param[in] my_mpi The MPI communicator to use for communication.
     *
     */
    ResourceSetPIMPL(size_type rank, mpi_comm_type my_mpi);

    /** @brief Makes a deep copy of *this.
     *
     * This method behaves identical to the copy ctor except that resulting
     * copy is on the heap and already wrapped up in a manner that facilitates
     * using it to power a new ResourceSet instance.
     *
     * @todo Should the Loggers be deep copied?
     *
     * @return A deep copy of *this allocated on the heap.
     */
    pimpl_pointer clone() const {
        return std::make_unique<ResourceSetPIMPL>(*this);
    }

    // -------------------------------------------------------------------------
    // -- Getters
    // -------------------------------------------------------------------------

    logger_reference progress_logger() {
        if(!m_progress_logger_pointer)
            throw std::runtime_error("No Progress Logger");
        return *m_progress_logger_pointer;
    }

    logger_reference debug_logger() {
        if(!m_debug_logger_pointer) throw std::runtime_error("No Debug Logger");
        return *m_debug_logger_pointer;
    }

    // -------------------------------------------------------------------------
    // -- Utility
    // -------------------------------------------------------------------------

    /** @brief Compares *this to another ResourceSetPIMPL instance
     *
     *  Two ResourceSetPIMPL instances if they are:
     *  - associated with the same MPI communicator,
     *  - owned by the same MPI rank of that communicator, and
     *  - have the same hardware resources.
     *
     *  N.B. In practice this method will implement both ResourceSet::operator==
     *  and ResourceSet::operator!=, since the latter is implemented by negating
     *  the former.
     *
     *  @param[in] rhs The instance we are comparing to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None no throw guarantee
     */
    bool operator==(const ResourceSetPIMPL& rhs) const noexcept;

    /// The rank of this process
    size_type m_rank;

    /// The RAM accessible to this process
    ram_type m_ram;

    /// The Runtime this resource set belongs to.
    mpi_comm_type m_my_mpi;

    /// Progress Logger
    logger_pointer m_progress_logger_pointer;

    /// Debug Logger
    logger_pointer m_debug_logger_pointer;
};

/** @brief Determines the size of the RAM local to the current process
 *
 *  This function wraps the process of figuring out how much RAM the current
 *  process has local access to.
 *
 *  @return The amount of RAM, hard-coded to 10
 *  @todo Implement me
 */
inline auto get_ram_size() {
    // TODO: Implement this for real
    return ResourceSetPIMPL::size_type(10);
}

// -----------------------------------------------------------------------------
// -- Inline Implementations
// -----------------------------------------------------------------------------

inline ResourceSetPIMPL::ResourceSetPIMPL(size_type rank,
                                          mpi_comm_type my_mpi) :
  m_rank(rank),
  m_ram(hardware::detail_::make_ram(get_ram_size(), rank, my_mpi)),
  m_my_mpi(my_mpi) {}

inline bool ResourceSetPIMPL::operator==(
  const ResourceSetPIMPL& rhs) const noexcept {
    // TODO: Compare loggers
    auto my_state  = std::tie(m_rank, m_ram, m_my_mpi);
    auto rhs_state = std::tie(rhs.m_rank, rhs.m_ram, rhs.m_my_mpi);

    return my_state == rhs_state;
}

} // namespace parallelzone::runtime::detail_
