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

TEST_CASE("runtime_view") {
    // 99.9% of the time you will be using an existing RuntimeView
    auto& rv = get_runtime();

    // Get the resource set containing resources local to the current process
    const auto& my_rs = rv.my_resource_set();

    // Get the MPI rank of the current process
    const auto my_rank = my_rs.mpi_rank();

    // "Compute some data"
    std::vector<std::size_t> local_data{my_rank, my_rank + 1, my_rank + 2};

    // Synchronize the data across the program
    auto results = rv.gather(local_data);

    // TODO: Print results using logger

    REQUIRE(results.size() == rv.size() * 3);

    std::vector<std::size_t> corr;
    for(std::size_t i = 0; i < rv.size(); ++i)
        for(std::size_t j = 0; j < 3; ++j) corr.push_back(i + j);
    REQUIRE(results == corr);
}
