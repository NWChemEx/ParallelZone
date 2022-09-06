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

#include <catch2/catch.hpp>
#include <parallelzone/hardware/ram.hpp>

using namespace parallelzone::hardware;

TEST_CASE("RAM") {
    RAM::size_type zero(0), max_size(100);
    RAM defaulted;
    RAM has_size(max_size);

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(defaulted.empty());
            REQUIRE(defaulted.total_space() == zero);
        }

        SECTION("total_size") {
            REQUIRE_FALSE(has_size.empty());
            REQUIRE(has_size.total_space() == max_size);
        }

        SECTION("copy") {
            RAM defaulted_copy(defaulted);
            REQUIRE(defaulted_copy == defaulted);

            RAM has_size_copy(has_size);
            REQUIRE(has_size_copy == has_size);
        }

        SECTION("move") {
            RAM defaulted_copy(defaulted);
            RAM defaulted_move(std::move(defaulted));
            REQUIRE(defaulted_copy == defaulted_move);

            RAM has_size_copy(has_size);
            RAM has_size_move(std::move(has_size));
            REQUIRE(has_size_copy == has_size_move);
        }

        SECTION("copy assignment") {
            RAM defaulted_copy;
            auto pdefaulted_copy = &(defaulted_copy = defaulted);
            REQUIRE(pdefaulted_copy == &defaulted_copy);
            REQUIRE(defaulted_copy == defaulted);

            RAM has_size_copy;
            auto phas_size_copy = &(has_size_copy = has_size);
            REQUIRE(phas_size_copy == &has_size_copy);
            REQUIRE(has_size_copy == has_size);
        }

        SECTION("move assignment") {
            RAM defaulted_copy(defaulted);
            RAM defaulted_move;
            auto pdefaulted_move = &(defaulted_move = std::move(defaulted));
            REQUIRE(pdefaulted_move == &defaulted_move);
            REQUIRE(defaulted_copy == defaulted_move);

            RAM has_size_copy(has_size);
            RAM has_size_move;
            auto phas_size_move = &(has_size_move = std::move(has_size));
            REQUIRE(phas_size_move == &has_size_move);
            REQUIRE(has_size_copy == has_size_move);
        }
    }

    SECTION("total_space") {
        REQUIRE(defaulted.total_space() == zero);
        REQUIRE(has_size.total_space() == max_size);
    }

    SECTION("gather") {
        REQUIRE_THROWS_AS(defaulted.gather(1.23), std::runtime_error);
    }

    SECTION("reduce") {
        REQUIRE_THROWS_AS(defaulted.reduce(1.23, 2.34), std::runtime_error);
    }

    SECTION("empty") {
        REQUIRE(defaulted.empty());
        REQUIRE_FALSE(has_size.empty());
    }

    SECTION("swap") {
        RAM defaulted_copy(defaulted);
        RAM has_size_copy(has_size);

        defaulted.swap(has_size);
        REQUIRE(defaulted == has_size_copy);
        REQUIRE(has_size == defaulted_copy);
    }

    SECTION("operator==/operator!=") {
        // Defaulted instances equal
        REQUIRE(defaulted == RAM());
        REQUIRE_FALSE(defaulted != RAM());

        // Defaulted is same as 0 memory
        REQUIRE(defaulted == RAM(0));
        REQUIRE_FALSE(defaulted != RAM(0));

        // Defaulted != non-default
        REQUIRE(defaulted != has_size);
        REQUIRE_FALSE(defaulted == has_size);

        // Same sizes
        REQUIRE(has_size == RAM(max_size));
        REQUIRE_FALSE(has_size != RAM(max_size));

        // Different sizes
        REQUIRE(has_size != RAM(zero));
        REQUIRE_FALSE(has_size == RAM(zero));
    }
}
