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
#include <parallelzone/runtime/resource_set.hpp>
#include <parallelzone/runtime/runtime_view.hpp>

namespace parallelzone::runtime::detail_ {

struct ResourceSetPIMPL {
    /// Type *this implements, typedef of ResourceSet
    using resource_set_type = ResourceSet;

    /// Type of the RuntimeView we're part of
    using runtime_view = RuntimeView;

    /// How the RuntimeView accesses MPI
    using mpi_comm_type = runtime_view::mpi_comm_type;

    using ram_type = resource_set_type::ram_type;

    using size_type = resource_set_type::size_type;

    using pimpl_pointer = resource_set_type::pimpl_pointer;

    using logger_type = resource_set_type::logger_type;

    using logger_reference = resource_set_type::logger_reference;

    using logger_pointer = std::shared_ptr<logger_type>;

    ResourceSetPIMPL(size_type rank, mpi_comm_type my_mpi);

    pimpl_pointer clone() const {
        return std::make_unique<ResourceSetPIMPL>(*this);
    }

    logger_reference progress_logger() {
        if(!m_progress_logger_pointer)
            throw std::runtime_error("No Progress Logger");
        return *m_progress_logger_pointer;
    }

    logger_reference debug_logger() {
        if(!m_debug_logger_pointer) throw std::runtime_error("No Debug Logger");
        return *m_debug_logger_pointer;
    }

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

inline ResourceSetPIMPL::ResourceSetPIMPL(size_type rank,
                                          mpi_comm_type my_mpi) :
  m_rank(rank), m_my_mpi(my_mpi) {
    using ram_pimpl = hardware::detail_::RAMPIMPL;

    // TODO: Find this out somehow
    size_type max_ram_size = 10;
    mpi_helpers::CommPP comm(my_mpi);
    auto pram =
      std::make_unique<ram_pimpl>(max_ram_size, rank, std::move(comm));
    ram_type(std::move(pram)).swap(m_ram);
}

} // namespace parallelzone::runtime::detail_
