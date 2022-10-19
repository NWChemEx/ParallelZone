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

TEST_CASE("quick start") {
    // 99.9% of the time you will be using an existing RuntimeView
    auto& rt = get_runtime();

    // Figure out how many resource sets there are total
    const auto n_rs = rt.size();

    // Get the resource set containing resources local to the current process
    const auto& my_rs = rt.my_resource_set();

    // Figure out the mpi rank of the current resource set
    const auto my_rs_rank = my_rs.mpi_rank();

    // Have every resource set send its index to resource set 0
    auto ranks_on_0 = rt.at(0).ram().gather(my_rs_rank);

    if(ranks_on_0) { // Only resource set 0 will enter this block
        // TODO: This should be the logger
        std::cout << "Received indices from " << ranks_on_0->size()
                  << " resource set(s)." << std::endl;
    }

    // Have every resource set send their rank to every other resource set
    auto ranks = rt.gather(my_rs_rank);

    // Figure out how much RAM the local process has access to
    const auto my_ram = my_rs.ram().total_space();

    REQUIRE(n_rs > 0);
    if(my_rs_rank == 0) {
        REQUIRE(ranks_on_0);
        REQUIRE(ranks_on_0->size() == n_rs);
    } else {
        REQUIRE_FALSE(ranks_on_0);
    }
}
