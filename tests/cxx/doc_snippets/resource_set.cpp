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

#include "test_parallelzone.hpp"

namespace {

auto& get_runtime() { return testing::PZEnvironment::comm_world(); }

} // namespace

TEST_CASE("resource_set") {
    // 99.9% of the time you will be using an existing RuntimeView
    auto& rv = get_runtime();

    // Get the ResourceSet for rank 0
    const auto& rank_0_rs = rv.at(0);

    // Get the resource set containing resources local to the current process
    const auto& my_rs = rv.my_resource_set();

    // Determine if the rank_0_rs is local or not
    const bool is_local = rank_0_rs.is_mine();

    // Get the local RAM
    const auto& my_ram = my_rs.ram();

    // Get a view of the RAM owned by rank 0
    const auto& ram_0 = rank_0_rs.ram();

    // Do I locally have RAM?
    const bool do_i_have_ram = my_rs.has_ram();

    // Does rank 0 have RAM?
    const bool does_0_have_ram = rank_0_rs.has_ram();

    const bool am_i_supposed_to_be_0 = my_rs.mpi_rank() == 0;
    REQUIRE(is_local == am_i_supposed_to_be_0);
    REQUIRE(do_i_have_ram);
    REQUIRE(does_0_have_ram);
}
