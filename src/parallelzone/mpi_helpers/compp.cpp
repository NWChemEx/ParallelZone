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

#include <parallelzone/mpi_helpers/commpp.hpp>
#include <stdexcept>

namespace parallelzone::mpi_helpers {

CommPP::CommPP(mpi_comm_type comm) : m_comm_(comm), m_my_rank_(0), m_size_(0) {
    MPI_Comm_rank(m_comm_, &m_my_rank_);
    MPI_Comm_size(m_comm_, &m_size_);
}

CommPP::mpi_comm_type CommPP::comm() const { return m_comm_; }

CommPP::size_type CommPP::size() const { return m_size_; }

CommPP::size_type CommPP::me() const { return m_my_rank_; }

CommPP::gather_return CommPP::gather(const_binary_reference data,
                                     size_type root) const {
    // Each rank sends n bytes, so if I'm root I get comm_size * n bytes.
    // All other ranks get nothing
    bool am_i_root = me() == root;
    int recv_size  = !am_i_root ? 0 : size() * data.size();
    binary_type buffer(recv_size);
    binary_reference pbuffer(buffer.data(), buffer.size());
    gather(data, pbuffer, root);
    gather_return rv;
    if(am_i_root) rv.emplace(std::move(buffer));
    return rv;
}

void CommPP::gather(const_binary_reference data, binary_reference out_buffer,
                    size_type root) const {
    auto p_in  = data.data();
    auto n_in  = data.size();
    auto p_out = out_buffer.data();
    if(me() == root && out_buffer.size() < n_in * size())
        throw std::runtime_error("The provided buffer is not large enough...");

    MPI_Gather(p_in, n_in, MPI_BYTE, p_out, n_in, MPI_BYTE, root, m_comm_);
}

} // namespace parallelzone::mpi_helpers
