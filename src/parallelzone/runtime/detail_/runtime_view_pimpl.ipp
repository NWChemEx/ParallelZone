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

inline void RuntimeViewPIMPL::stack_callback(
  std::function<void()> cb_func, std::stack<std::function<void()>> m_stack) {
    m_stack.push(cb_func);
}

void mpi_finalize_wrapper() { MPI_Finalize(); }

inline RuntimeViewPIMPL::RuntimeViewPIMPL(bool did_i_start_mpi, comm_type comm,
                                          logger_type logger) :
  m_did_i_start_mpi(did_i_start_mpi),
  m_comm(comm),
  m_plogger(std::make_shared<logger_type>(std::move(logger))),
  m_resource_sets_() {
    // Pre-populate the current rank's resource set.
    instantiate_resource_set_(m_comm.me());

    /// Register the finalize callbacks
    stack_callback(std::function<void()>{&mpi_finalize_wrapper},
                   m_callbacks_final);
}

inline RuntimeViewPIMPL::~RuntimeViewPIMPL() noexcept {
    if(!m_did_i_start_mpi) return;
    // MPI_Finalize();

    /// call the initialize callback functions
    while(!m_callbacks_final.empty()) {
        m_callbacks_final.top()();
        m_callbacks_final.pop();
    }
}

inline RuntimeViewPIMPL::const_resource_set_reference RuntimeViewPIMPL::at(
  size_type rank) const {
    instantiate_resource_set_(rank);
    return m_resource_sets_.at(rank);
}

inline bool RuntimeViewPIMPL::operator==(
  const RuntimeViewPIMPL& rhs) const noexcept {
    // Except for the logger, all other RuntimeViewPIMPL state is tied to the
    // MPI communicator
    if(m_comm != rhs.m_comm) return false;
    return *m_plogger == *rhs.m_plogger;
}

void RuntimeViewPIMPL::instantiate_resource_set_(size_type rank) const {
    using rs_pimpl = detail_::ResourceSetPIMPL;
    if(m_resource_sets_.count(rank)) return;

    // Null loggers for now
    logger_type logger;

    auto p = std::make_unique<rs_pimpl>(rank, m_comm, std::move(logger));
    m_resource_sets_.emplace(rank, ResourceSet(std::move(p)));
}

} // namespace parallelzone::runtime::detail_
