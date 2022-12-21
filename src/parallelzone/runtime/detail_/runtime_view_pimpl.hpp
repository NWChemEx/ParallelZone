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
#include <parallelzone/runtime/runtime_view.hpp>

namespace parallelzone::runtime::detail_ {

/** @brief Holds the state for the RuntimeView class
 *
 *  Right now this class assumes MADNESS is managing MPI, so it holds a MADNESS
 *  world; however, no part of ParallelZone actually uses the MADNESS world
 *  and it is entirely possible to just power this class off of the CommPP
 *  object.
 *
 *  One option going forward would be to make this class polymorphic and have
 *  the derived classes power the RuntimeView based on the runtime the user
 *  wants to use (e.g., one derived class if we want to use MADNESS, another
 *  derived class for CommPP).
 *
 */
struct RuntimeViewPIMPL {
    /// Type of the class this PIMPL implements
    using parent_type = RuntimeView;

    /// Ultimately a typedef of RuntimeView::size_type
    using size_type = parent_type::size_type;

    /// Ultimately a typedef of RuntimeView::resource_set_type
    using resource_set_type = parent_type::resource_set_type;

    /// Ultimately a typedef of RuntimeView::resource_set_reference
    using resource_set_reference = parent_type::resource_set_reference;

    /// Ultimately a typedef of RuntimeView::const_resource_set_reference
    using const_resource_set_reference =
      parent_type::const_resource_set_reference;

    /// Type of the conatiner holding resource_set_type objects
    using resource_set_container = std::map<size_type, resource_set_type>;

    /// Ultimately a typedef of RuntimeView::madness_world_reference
    using madness_world_reference = parent_type::madness_world_reference;

    /// The type of our MPI Comm wrapper
    using comm_type = mpi_helpers::CommPP;

    /// Logger type
    using logger_type = parent_type::logger_type;

    /// Read/write Logger reference
    using logger_reference = parent_type::logger_reference;

    /// Logger instance pointer
    using logger_pointer = std::shared_ptr<logger_type>;

    /// Ultimately a typedef of RuntimeView::argc_type
    using argc_type = parent_type::argc_type;

    /// Ultimately a typedef of RuntimeView::argv_type
    using argv_type = parent_type::argv_type;

    /** @brief Initializes *this from the provided MADNESS world.
     *
     *  This ctor will strip the relevant information off of the MADNESS world
     *  to initialize *this (e.g., the rank of the current process, and the MPI
     *  communicator). It also records whether or no *this is responsible for
     *  tearing down MADNESS when it goes out of scope (if *this started
     *  MADNESS then it is responsible for tearing it down).
     *
     *  @param[in] did_i_start_madness True if *this should be responsible for
     *                                 the lifetime of MADNESS and false
     *                                 otherwise.
     *  @param[in] world The MADNESS world *this wraps.
     *
     */
    RuntimeViewPIMPL(bool did_i_start_madness, madness_world_reference world);

    /// Tears down MADNESS, when all references are gone (and if we started it)
    ~RuntimeViewPIMPL() noexcept;

    /** @brief Wraps retrieving a ResourceSet
     *
     *  Behind the scenes there's a bit of redirection involved in the storage
     *  of ResourceSet instances. This function hides that from the caller.
     *
     *  @param[in] rank The rank whose resource set is wanted.
     *
     *  @return A read-only reference to the requested ResourceSet.
     *
     *  @throw std::bad_alloc if the ResourceSet has not been instantiated
     *                        prior to this call and there's a problem
     *                        allocating it.
     */
    const_resource_set_reference at(size_type rank) const;

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two RuntimeViewPIMPL instances are value equal if they both wrap the
     *  same MPI communicator and if they both have access to the same resource
     *  sets. At the moment the content of the resource sets is populated off
     *  of the MPI communicator so we only compare the MPI communicators.
     *
     *  Eventually the user will be able to logically partition the RuntimeView
     *  and at this time operator== will need revisited.
     *
     *  N.B. This class only defines operator== because that is all that is
     *  needed for RuntimeView to define both operator== and operator!=.
     *
     *  @param[in] rhs The RuntimeView we are comparing to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const RuntimeViewPIMPL& rhs) const noexcept;

    /// Did this PIMPL start MADNESS?
    bool m_did_i_start_madness;

    /// Reference to the madness world this instance wraps
    madness_world_reference m_world;

    /// The MPI communicator we're built around
    comm_type m_comm;

    /// Progress Logger
    logger_pointer m_progress_logger_pointer;

    /// Debug Logger
    logger_pointer m_debug_logger_pointer;

private:
    /** @brief Wraps the process of instantiating a ResourceSet.
     *
     *  When a user requests a ResourceSet they get a reference to it. This
     *  method will ensure that there is an instance to return by reference.
     *  This function is a no-op if the requested ResourceSet has already been
     *  instantiated.
     *
     *  N.B. This function does not perform a bounds check and defers that to
     *  the RuntimeView class.
     *
     *  @param[in] rank The rank whose resource set is needed.
     *
     *  @throw std::bad_alloc if a new ResourceSet instance is needed and there
     *                        is a problem creating that instance.
     */
    void instantiate_resource_set_(size_type rank) const;

    /** @brief The ResourceSets known to this RuntimeView
     *
     *  In practice most MPI ranks are only going to care about their
     *  ResourceSet. So instead of making an `n`-element vector where
     *  element `i` is the resource set for rank `i` (`n` is the number of
     *  ranks), we instead store them in a map. Conceptually, we think of the
     *  map as storing all `n` resource sets, but some of them are stored
     *  implicitly. By default we only populate the resource set for the
     *  current rank.
     *
     *  This member is mutable so that if we need to explicitly instantiate a
     *  ResourceSet in a const function we can do that.
     */
    mutable resource_set_container m_resource_sets_;
};

} // namespace parallelzone::runtime::detail_

#include "runtime_view_pimpl.ipp"
