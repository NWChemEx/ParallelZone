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
#include <parallelzone/hardware/ram/ram.hpp>

using namespace parallelzone::hardware;

/* Testing Strategy:
 *
 * RAM instances with value are really meant to be created by resource sets.
 *
 */

TEST_CASE("RAM") {
    using ram_type  = RAM;
    using size_type = ram_type::size_type;
    const auto& run = testing::PZEnvironment::comm_world();
    const auto& rs  = run.my_resource_set();

    RAM defaulted;
    RAM has_value = rs.ram();

    size_type zero(0);

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(defaulted.empty());
            REQUIRE(defaulted.total_space() == zero);
        }

        SECTION("Value") {
            REQUIRE_FALSE(has_value.empty());
            REQUIRE(has_value.total_space() > 0);
        }

        SECTION("copy") {
            RAM defaulted_copy(defaulted);
            REQUIRE(defaulted_copy == defaulted);

            RAM has_value_copy(has_value);
            REQUIRE(has_value_copy == has_value);
        }

        SECTION("move") {
            RAM defaulted_copy(defaulted);
            RAM defaulted_move(std::move(defaulted));
            REQUIRE(defaulted_copy == defaulted_move);

            RAM has_value_copy(has_value);
            RAM has_value_move(std::move(has_value));
            REQUIRE(has_value_copy == has_value_move);
        }

        SECTION("copy assignment") {
            RAM defaulted_copy;
            auto pdefaulted_copy = &(defaulted_copy = defaulted);
            REQUIRE(pdefaulted_copy == &defaulted_copy);
            REQUIRE(defaulted_copy == defaulted);

            RAM has_value_copy;
            auto phas_value_copy = &(has_value_copy = has_value);
            REQUIRE(phas_value_copy == &has_value_copy);
            REQUIRE(has_value_copy == has_value);
        }

        SECTION("move assignment") {
            RAM defaulted_copy(defaulted);
            RAM defaulted_move;
            auto pdefaulted_move = &(defaulted_move = std::move(defaulted));
            REQUIRE(pdefaulted_move == &defaulted_move);
            REQUIRE(defaulted_copy == defaulted_move);

            RAM has_value_copy(has_value);
            RAM has_value_move;
            auto phas_value_move = &(has_value_move = std::move(has_value));
            REQUIRE(phas_value_move == &has_value_move);
            REQUIRE(has_value_copy == has_value_move);
        }
    }

    SECTION("total_space") {
        REQUIRE(defaulted.total_space() == zero);
        REQUIRE(has_value.total_space() > 0);
    }

    SECTION("gather") {
        using data_type = std::vector<std::string>;
        data_type local_data(3, "Hello");
        auto rv = run.at(0).ram().gather(local_data);
        if(run.at(0).is_mine()) {
            std::vector<data_type> corr(run.size(), local_data);
            REQUIRE(rv.has_value());
            REQUIRE(*rv == corr);
        } else {
            REQUIRE_FALSE(rv.has_value());
        }
    }

    SECTION("reduce") {
        using data_type = std::vector<double>;
        data_type local_data(3, 1.0);
        auto op = std::plus<double>();
        auto rv = run.at(0).ram().reduce(local_data, op);

        if(run.at(0).is_mine()) {
            data_type corr(3, run.size());
            REQUIRE(rv.has_value());
            REQUIRE(*rv == corr);
        } else {
            REQUIRE_FALSE(rv.has_value());
        }
    }

    SECTION("empty") {
        REQUIRE(defaulted.empty());
        REQUIRE_FALSE(has_value.empty());
    }

    SECTION("swap") {
        RAM defaulted_copy(defaulted);
        RAM has_value_copy(has_value);

        defaulted.swap(has_value);
        REQUIRE(defaulted == has_value_copy);
        REQUIRE(has_value == defaulted_copy);
    }

    SECTION("operator==/operator!=") {
        // Defaulted instances equal
        REQUIRE(defaulted == RAM());
        REQUIRE_FALSE(defaulted != RAM());

        // Defaulted != non-default
        REQUIRE(defaulted != has_value);
        REQUIRE_FALSE(defaulted == has_value);
    }
}
