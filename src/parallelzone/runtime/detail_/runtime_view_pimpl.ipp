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
#include "resource_set_pimpl.hpp"

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
  m_comm(world.mpi.comm().Get_mpi_comm()),
  m_resource_sets_() {
    // Pre-populate the current rank's resource set.
    instantiate_resource_set_(m_comm.me());
}

inline RuntimeViewPIMPL::~RuntimeViewPIMPL() noexcept {
    if(!m_did_i_start_madness) return;
    madness::finalize();
}

inline RuntimeViewPIMPL::const_resource_set_reference RuntimeViewPIMPL::at(
  size_type rank) const {
    instantiate_resource_set_(rank);
    return m_resource_sets_.at(rank);
}

inline bool RuntimeViewPIMPL::operator==(
  const RuntimeViewPIMPL& rhs) const noexcept {
    // Right now the state of all resource sets is tied to the MPI communicator
    // and can't be mutated. In turn, if two RuntimeViewPIMPL instances wrap the
    // same communicator they also have the same resource sets.
    return m_comm == rhs.m_comm;
}

void RuntimeViewPIMPL::instantiate_resource_set_(size_type rank) const {
    if(m_resource_sets_.count(rank)) return;
    auto p = std::make_unique<detail_::ResourceSetPIMPL>(rank, m_comm);
    m_resource_sets_.emplace(rank, ResourceSet(std::move(p)));
}

} // namespace parallelzone::runtime::detail_
