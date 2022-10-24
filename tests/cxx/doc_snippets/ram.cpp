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
}
