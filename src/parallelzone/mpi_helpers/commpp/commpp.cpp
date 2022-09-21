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

#include "detail_/commpp_pimpl.hpp"
#include <parallelzone/mpi_helpers/commpp/commpp.hpp>
#include <stdexcept>

namespace parallelzone::mpi_helpers {

// -----------------------------------------------------------------------------
// -- CTors, Assignment, and Dtor
// -----------------------------------------------------------------------------

CommPP::CommPP() noexcept = default;

CommPP::CommPP(mpi_comm_type comm) :
  m_pimpl_(std::make_unique<pimpl_type>(comm)) {}

CommPP::CommPP(const CommPP& other) :
  m_pimpl_(other.has_pimpl_() ? other.m_pimpl_->clone() : nullptr) {}

CommPP::~CommPP() noexcept = default;

CommPP::mpi_comm_type CommPP::comm() const noexcept {
    return has_pimpl_() ? m_pimpl_->comm() : MPI_COMM_NULL;
}

CommPP::size_type CommPP::size() const noexcept {
    return has_pimpl_() ? m_pimpl_->size() : 0;
}

CommPP::size_type CommPP::me() const noexcept {
    return has_pimpl_() ? m_pimpl_->me() : MPI_PROC_NULL;
}

// -----------------------------------------------------------------------------
// -- Utility Methods
// -----------------------------------------------------------------------------

bool CommPP::operator==(const CommPP& rhs) const noexcept {
    if(has_pimpl_() != rhs.has_pimpl_()) return false;
    if(!has_pimpl_()) return true; // Both Null comm
    return *m_pimpl_ == *rhs.m_pimpl_;
}

bool CommPP::operator!=(const CommPP& rhs) const noexcept {
    return !(*this == rhs);
}

// -----------------------------------------------------------------------------
// -- Private Methods
// -----------------------------------------------------------------------------

bool CommPP::has_pimpl_() const noexcept { return static_cast<bool>(m_pimpl_); }

const CommPP::pimpl_type& CommPP::pimpl_() const {
    if(has_pimpl_()) return *m_pimpl_;
    throw std::runtime_error("CommPP does not have a PIMPL.");
}

CommPP::binary_gather_return CommPP::gather_(const_binary_reference data,
                                             opt_root_t root) const {
    return pimpl_().gather(data, root);
}

void CommPP::gather_(const_binary_reference data, binary_reference out_buffer,
                     opt_root_t root) const {
    pimpl_().gather(data, out_buffer, root);
}

// CommPP::binary_gatherv_return CommPP::gatherv_binary(
//   const_binary_reference data, size_type root) const {
//     return pimpl_().gatherv(data, root);
// }

} // namespace parallelzone::mpi_helpers
