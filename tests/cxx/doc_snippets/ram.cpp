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

TEST_CASE("ram") {
    // 99.9% of the time you will be using an existing RuntimeView
    auto& rv = get_runtime();

    // Get the RAM for rank 0
    const auto& rank_0_ram = rv.at(0).ram();

    // Get the RAM local to the current process
    const auto& my_ram = rv.my_resource_set().ram();

    // How much total ram does rank_0_ram and my_ram have?
    const auto rank_0_total = rank_0_ram.total_space();
    const auto my_total_ram = my_ram.total_space();

    // Generate some process-specific data
    const auto my_rank = rv.my_resource_set().mpi_rank();
    std::vector<std::size_t> local_data{my_rank, my_rank + 1, my_rank + 2};

    auto all_data = rank_0_ram.gather(local_data);

    if(all_data) {
        // TODO: use logger
    } else {
        // TODO: use logger
    }

    std::vector<std::size_t> corr;
    for(std::size_t i = 0; i < rv.size(); ++i)
        for(std::size_t j = 0; j < 3; ++j) corr.push_back(i + j);

    if(my_rank == 0) {
        REQUIRE(corr == *all_data);
    } else {
        REQUIRE_FALSE(all_data);
    }
}
