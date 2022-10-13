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

#include "detail_/resource_set_pimpl.hpp"

namespace parallelzone::runtime {

// -----------------------------------------------------------------------------
// -- Ctors, assignment, and dtor
// -----------------------------------------------------------------------------

ResourceSet::ResourceSet() noexcept = default;

ResourceSet::ResourceSet(pimpl_pointer pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

ResourceSet::ResourceSet(const ResourceSet& other) :
  m_pimpl_(other.m_pimpl_ ? other.m_pimpl_->clone() : nullptr) {}

ResourceSet::ResourceSet(ResourceSet&& other) noexcept = default;

ResourceSet& ResourceSet::operator=(const ResourceSet& rhs) {
    if(this != &rhs) { ResourceSet(rhs).swap(*this); }
    return *this;
}

ResourceSet& ResourceSet::operator=(ResourceSet&& rhs) noexcept = default;

ResourceSet::~ResourceSet() noexcept = default;

// -----------------------------------------------------------------------------
// -- Getters
// -----------------------------------------------------------------------------

ResourceSet::size_type ResourceSet::mpi_rank() const noexcept {
    return has_pimpl_() ? m_pimpl_->m_rank : MPI_PROC_NULL;
}

bool ResourceSet::is_mine() const noexcept {
    if(!has_pimpl_()) return false;
    if(mpi_rank() == MPI_PROC_NULL) return false;
    return m_pimpl_->m_my_mpi.me() == mpi_rank();
}

bool ResourceSet::has_ram() const noexcept { return has_pimpl_() && !null(); }

ResourceSet::const_ram_reference ResourceSet::ram() const {
    if(has_ram()) return m_pimpl_->m_ram;
    throw std::out_of_range("ResourceSet has no RAM");
}

ResourceSet::logger_reference ResourceSet::progress_logger() {
    assert_pimpl_();
    return m_pimpl_->progress_logger();
}

ResourceSet::logger_reference ResourceSet::debug_logger() {
    assert_pimpl_();
    return m_pimpl_->debug_logger();
}

// -----------------------------------------------------------------------------
// -- Setters
// -----------------------------------------------------------------------------

void ResourceSet::set_progress_logger(logger_type&& l) {
    assert_pimpl_();
    m_pimpl_->m_progress_logger_pointer =
      std::make_unique<logger_type>(std::move(l));
}

void ResourceSet::set_debug_logger(logger_type&& l) {
    assert_pimpl_();
    m_pimpl_->m_debug_logger_pointer =
      std::make_unique<logger_type>(std::move(l));
}

// -----------------------------------------------------------------------------
// -- Utility methods
// -----------------------------------------------------------------------------

bool ResourceSet::null() const noexcept { return mpi_rank() == MPI_PROC_NULL; }

bool ResourceSet::empty() const noexcept {
    // TODO: Need to check for loggers
    return null() || !has_ram();
}

void ResourceSet::swap(ResourceSet& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

bool ResourceSet::operator==(const ResourceSet& rhs) const noexcept {
    // Check if one is null and other isn't
    if(null() != rhs.null()) return false;

    // Both are null, or both are non-null, if null return
    if(null()) return true;

    return *m_pimpl_ == *rhs.m_pimpl_;
}

bool ResourceSet::operator!=(const ResourceSet& rhs) const noexcept {
    return !(*this == rhs);
}

// -----------------------------------------------------------------------------
// -- Protected/Private methods
// -----------------------------------------------------------------------------

bool ResourceSet::has_pimpl_() const noexcept {
    return static_cast<bool>(m_pimpl_);
}

void ResourceSet::assert_pimpl_() const {
    if(has_pimpl_()) return;
    throw std::runtime_error(
      "ResourceSet has no PIMPL. Was it default constructed or moved from?");
}

} // namespace parallelzone::runtime
