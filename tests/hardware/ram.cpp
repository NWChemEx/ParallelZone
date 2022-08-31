#include "../test_parallelzone.hpp"
#include <parallelzone/hardware/ram.hpp>

using namespace parallelzone::hardware;

/* Testing Strategy:
 *
 * RAM instances with value are really meant to be created by resource sets.
 *
 */

TEST_CASE("RAM") {
    using ram_type  = RAM;
    using size_type = ram_type::size_type;
    const auto& rs  = testing::PZEnvironment::comm_world().my_resource_set();

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
        REQUIRE_THROWS_AS(defaulted.gather(1.23), std::runtime_error);
    }

    SECTION("reduce") {
        REQUIRE_THROWS_AS(defaulted.reduce(1.23, 2.34), std::runtime_error);
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
