#pragma once

/** @file runtime_view_pimpl.ipp
 *
 *  This file contains inline definitions of methods for the RuntimeViewPIMPL
 *  class. It is meant only for inclusion by RuntimeViewPIMPL.hpp
 */

namespace parallelzone::runtime::detail_ {

inline RuntimeViewPIMPL::RuntimeViewPIMPL(bool did_i_start_madness,
                                          madness_world_reference world) :
  m_did_i_start_madness(did_i_start_madness),
  m_world(world),
  m_comm(world.mpi.comm()),
  m_progress_logger_pointer(make_default_stdout_logger(world)),
  m_debug_logger_pointer(make_default_stderr_logger(world)),
  m_resource_sets_() {}

inline RuntimeViewPIMPL::~RuntimeViewPIMPL() noexcept {
    if(!m_did_i_start_madness) return;
    madness::finalize();
}

inline RuntimeViewPIMPL::const_resource_set_reference RuntimeViewPIMPL::at(
  size_type rank) const {
    instantiate_resource_set_(rank);
    return m_resource_set.at(rank);
}

inline bool RuntimeViewPIMPL::operator==(
  const RuntimeViewPIMPL& rhs) const noexcept {
    // Right now the state of all resource sets is tied to the MPI communicator
    // and can't be mutated. In turn, if two RuntimeViewPIMPL instances wrap the
    // same communicator they also have the same resource sets.
    return m_comm == rhs.m_comm;
}

void RuntimeViewPIMPL::instantiate_resource_set_(size_type rank) const {
    if(m_resource_sets.count(rank)) return;
    auto p = std::make_unique<detail_::ResourceSetPIMPL>(rank, m_comm);
    m_resource_sets.emplace(rank, ResourceSet(std::move(p)));
}

} // namespace parallelzone::runtime::detail_
