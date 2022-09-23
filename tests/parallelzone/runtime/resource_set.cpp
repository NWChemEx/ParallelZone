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
#include "../test_parallelzone.hpp"
#include <parallelzone/runtime/resource_set.hpp>

using namespace parallelzone::runtime;

/* Testing Notes:
 *
 * In practice ResourceSets are always affiliated with a RuntimeView. Thus to
 * get a filled in ResourceSet it's easiest to grab one from the current
 * RuntimeView.
 */
TEST_CASE("ResourceSet") {
    const auto& rs = testing::PZEnvironment::comm_world().my_resource_set();
    ResourceSet defaulted;

    SECTION("Ctors and assignment") {
        SECTION("Default") {}

        SECTION("Copy") {
            ResourceSet defaulted_copy(defaulted);
            REQUIRE(defaulted_copy == defaulted);
        }

        SECTION("Move") {
            ResourceSet defaulted_copy(defaulted);
            ResourceSet defaulted_move(std::move(defaulted));
            REQUIRE(defaulted_copy == defaulted_move);
        }

        SECTION("Copy Assignment") {
            ResourceSet defaulted_copy;
            auto pdefaulted_copy = &(defaulted_copy = defaulted);
            REQUIRE(pdefaulted_copy == &defaulted_copy);
            REQUIRE(defaulted_copy == defaulted);
        }

        SECTION("Move Assignment") {
            ResourceSet defaulted_copy(defaulted);
            ResourceSet defaulted_move;
            auto pdefaulted_move = &(defaulted_move = std::move(defaulted));
            REQUIRE(pdefaulted_move == &defaulted_move);
            REQUIRE(defaulted_copy == defaulted_move);
        }
    }

    SECTION("mpi_rank") {
        REQUIRE_THROWS_AS(defaulted.mpi_rank(), std::runtime_error);
    }

    SECTION("is_mine") {
        REQUIRE_FALSE(defaulted.is_mine());
        REQUIRE(rs.is_mine());
    }

    SECTION("has_ram") { REQUIRE_FALSE(defaulted.has_ram()); }

    SECTION("ram") { REQUIRE_THROWS_AS(defaulted.ram(), std::out_of_range); }

    SECTION("empty") { REQUIRE(defaulted.empty()); }

    SECTION("swap") {
        // TODO: test when we have ResourceSets with different state
    }

    SECTION("operator==/operator!=") {
        ResourceSet other_defaulted;
        REQUIRE(defaulted == other_defaulted);
        REQUIRE_FALSE(defaulted != other_defaulted);

        // TODO: Test non-empty
    }

    SECTION("progress_logger") {
        // TODO: Set and test progress logger
        REQUIRE_THROWS_AS(defaulted.progress_logger(), std::runtime_error);
    }

    SECTION("debug_logger") {
        // TODO: Set and test debug logger
        REQUIRE_THROWS_AS(defaulted.debug_logger(), std::runtime_error);
    }
}
