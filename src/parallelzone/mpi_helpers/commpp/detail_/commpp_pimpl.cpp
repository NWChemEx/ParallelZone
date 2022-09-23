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

#include "commpp_pimpl.hpp"

namespace parallelzone::mpi_helpers::detail_ {

CommPPPIMPL::CommPPPIMPL(mpi_comm_type comm) :
  m_comm_(comm), m_my_rank_(0), m_size_(0) {
    MPI_Comm_rank(m_comm_, &m_my_rank_);
    MPI_Comm_size(m_comm_, &m_size_);
}

// -----------------------------------------------------------------------------
// -- MPI Operations
// -----------------------------------------------------------------------------

CommPPPIMPL::binary_gather_return CommPPPIMPL::gather(
  const_binary_reference data, opt_root_t root) const {
    // Everybody is root if root is not provided
    bool am_i_root = root.has_value() ? me() == *root : true;

    // Each rank sends n bytes, so if I'm root I get comm_size * n bytes.
    // All other ranks get nothing
    int recv_size = !am_i_root ? 0 : size() * data.size();
    binary_type buffer(recv_size);
    binary_reference pbuffer(buffer.data(), buffer.size());
    gather(data, pbuffer, root);
    binary_gather_return rv;
    if(am_i_root) rv.emplace(std::move(buffer));
    return rv;
}

void CommPPPIMPL::gather(const_binary_reference data,
                         binary_reference out_buffer, opt_root_t root) const {
    // Everybody is root if root is not provided
    auto am_i_root = root.has_value() ? me() == *root : true;

    auto p_in  = data.data();
    auto n_in  = data.size();
    auto p_out = out_buffer.data();
    if(am_i_root && out_buffer.size() < n_in * size())
        throw std::runtime_error("The provided buffer is not large enough...");

    if(root.has_value()) {
        MPI_Gather(p_in, n_in, MPI_BYTE, p_out, n_in, MPI_BYTE, *root, m_comm_);
    } else {
        MPI_Allgather(p_in, n_in, MPI_BYTE, p_out, n_in, MPI_BYTE, m_comm_);
    }
}

CommPPPIMPL::binary_gatherv_return CommPPPIMPL::gatherv(
  const_binary_reference data, opt_root_t root) const {
    const bool am_i_root = root.has_value() ? me() == *root : true;

    auto p_in = data.data();
    int n_in  = data.size(); // Need the 'int' to convert from std::size_t

    // Step 0: Gather the data sizes (in bytes) to the root, result is 'sizes'
    const_binary_reference local_size(&n_in, 1);
    std::vector<int> sizes;
    if(am_i_root) std::vector<int>(size(), 0).swap(sizes);
    binary_reference size_buffer(sizes.data(), sizes.size());
    gather(local_size, size_buffer, root);

    // Step 1: On root compute displacements and allocate buffer for gathered
    //         results. N.B. p_recv + disp[i] = address where rank i's data goes
    std::vector<int> disp;
    binary_type buffer;
    if(am_i_root) {
        int total = 0;
        // In our case rank i's results go immediately after rank (i-1)'s
        for(size_type i = 0; i < size(); ++i) {
            disp.push_back(total);
            total += sizes[i];
        }
        binary_type(std::size_t(total)).swap(buffer);
    }

    // Step 2: Do the gatherv/all gatherv
    auto* p_out        = buffer.data();
    const auto* p_recv = sizes.data();
    const auto* p_disp = disp.data();
    auto byte          = MPI_BYTE;
    if(root.has_value()) {
        MPI_Gatherv(p_in, n_in, byte, p_out, p_recv, p_disp, byte, *root,
                    m_comm_);
    } else {
        MPI_Allgatherv(p_in, n_in, byte, p_out, p_recv, p_disp, byte, m_comm_);
    }

    // Step 3: Return buffer and sizes
    binary_gatherv_return rv;
    if(am_i_root) {
        auto pair = std::make_pair(std::move(buffer), std::move(sizes));
        rv.emplace(std::move(pair));
    }
    return rv;
}

// -----------------------------------------------------------------------------
// -- Utility functions
// -----------------------------------------------------------------------------

bool CommPPPIMPL::operator==(const CommPPPIMPL& rhs) const noexcept {
    // N.B. Since the comparison is process local, and *this always describes
    //      the local process, we don't need to check m_my_rank_ or m_size_

    int result;
    MPI_Comm_compare(m_comm_, rhs.m_comm_, &result);
    return result == MPI_IDENT;
}

} // namespace parallelzone::mpi_helpers::detail_
