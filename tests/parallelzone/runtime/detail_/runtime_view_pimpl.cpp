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
    auto& rt = testing::PZEnvironment::comm_world();
    RuntimeViewPIMPL::comm_type comm(rt.madness_world().mpi.comm());
    RuntimeViewPIMPL::comm_type null_comm;
    RuntimeViewPIMPL pimpl(false, rt.madness_world());

    SECTION("CTor") {
        REQUIRE_FALSE(pimpl.m_did_i_start_madness);
        REQUIRE(pimpl.m_comm == comm);
    }

    SECTION("at") {
        auto p = std::make_unique<detail_::ResourceSetPIMPL>(0, comm);
        REQUIRE(did_not_start.at(0) == ResourceSet(std::move(p)));
    }

    SECTION("operator==") {
        RuntimeViewPIMPL other(false, rt.madness_world());
        REQUIRE(pimpl == other);
    }
}
