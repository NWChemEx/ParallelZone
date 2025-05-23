/*
 * Copyright 2025 NWChemEx-Project
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

#include "../../catch.hpp"
#include <iostream>
#include <numeric>
#include <parallelzone/hardware/cpu/cpu.hpp>

using namespace parallelzone;

TEST_CASE("CPU") {
    hardware::CPU defaulted;

    SECTION("profile_it") {
        using vector_type = std::vector<int>;
        vector_type a_vector{1, 2, 3};
        auto pa_vector = a_vector.data();

        SECTION("No return") {
            auto l = [pa_vector](vector_type v) {
                REQUIRE(v.data() == pa_vector);
            };

            auto info = defaulted.profile_it(l, std::move(a_vector));
            REQUIRE(info.wall_time.count() > 0); // Should have taken time...
        }

        SECTION("Has return") {
            auto l = [pa_vector](vector_type v) {
                REQUIRE(v.data() == pa_vector);
                return v;
            };

            auto&& [rv, info] = defaulted.profile_it(l, std::move(a_vector));
            REQUIRE(rv.data() == pa_vector); // Test there's no hidden copies
            REQUIRE(info.wall_time.count() > 0); // Should have taken time...
        }
    }
}