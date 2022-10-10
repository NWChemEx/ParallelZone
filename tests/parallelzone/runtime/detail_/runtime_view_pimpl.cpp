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

#include "../../test_parallelzone.hpp"
#include <parallelzone/runtime/detail_/runtime_view_pimpl.hpp>

using namespace parallelzone::runtime;
using namespace parallelzone::runtime::detail_;

/* Testing Notes
 *
 * Unfortunately fully testing the ctor/dtor is tricky because we can't restart
 * MADNESS/MPI. In particular this means if we make a
 * RuntimeViewPIMPL instance and tell it that it started MADNESS, when it goes
 * out of scope it will finalize MADNESS and break all of the other tests in
 * ParallelZone (unless it just so happens that this is the only test run or
 * the last one).
 *
 */

TEST_CASE("RuntimeViewPIMPL") {
    auto& rt    = testing::PZEnvironment::comm_world();
    auto& world = rt.madness_world();
    RuntimeViewPIMPL::comm_type comm(world.mpi.comm().Get_mpi_comm());
    RuntimeViewPIMPL::comm_type null_comm;
    RuntimeViewPIMPL pimpl(false, rt.madness_world());

    SECTION("CTor") {
        REQUIRE_FALSE(pimpl.m_did_i_start_madness);
        REQUIRE(pimpl.m_comm == comm);
    }

    SECTION("at") {
        auto p = std::make_unique<ResourceSetPIMPL>(0, comm);
        REQUIRE(pimpl.at(0) == ResourceSet(std::move(p)));

        if(comm.size() > 1) {
            auto p2 = std::make_unique<ResourceSetPIMPL>(1, comm);
            REQUIRE(pimpl.at(1) == ResourceSet(std::move(p2)));
        }
    }

    SECTION("operator==") {
        RuntimeViewPIMPL other(false, rt.madness_world());
        REQUIRE(pimpl == other);

        other.m_comm = RuntimeViewPIMPL::comm_type{};
        REQUIRE_FALSE(pimpl == other);
    }
}
