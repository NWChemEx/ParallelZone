#pragma once
#include <parallelzone/runtime/resource_set.hpp>
#include <parallelzone/runtime/runtime_view.hpp>

namespace parallelzone::runtime::detail_ {

struct ResourceSetPIMPL {
    using resource_set_type = ResourceSet;

    using madness_world_reference = RuntimeView::madness_world_reference;

    using ram_type = resource_set_type::ram_type;

    using size_type = resource_set_type::size_type;

    using pimpl_pointer = resource_set_type::pimpl_pointer;

    ResourceSetPIMPL(size_type rank, madness_world_reference world) :
      m_rank(rank), m_world(world) {}

    pimpl_pointer clone() const {
        return std::make_unique<ResourceSetPIMPL>(*this);
    }

    /// The rank of this process
    size_type m_rank;

    /// The RAM accessible to this process
    ram_type m_ram;

    madness_world_reference m_world;
};

} // namespace parallelzone::runtime::detail_
