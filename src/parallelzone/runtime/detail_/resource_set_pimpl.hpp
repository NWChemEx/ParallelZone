#pragma once
#include "../../hardware/detail_/ram_pimpl.hpp"
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
    auto pram = std::make_unique<ram_pimpl>(max_ram_size, rank, my_mpi);
    ram_type(std::move(pram)).swap(m_ram);
}

} // namespace parallelzone::runtime::detail_
