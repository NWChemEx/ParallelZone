#include <catch2/catch.hpp>
#include <parallelzone/runtime/resource_set.hpp>

using namespace parallelzone::runtime;

TEST_CASE("ResourceSet") {
    ResourceSet defaulted;

    SECTION("Ctors and assignment") {
        SECTION("Default") { REQUIRE(defaulted.n_workers() == 0); }

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

    SECTION("n_workers") { REQUIRE(defaulted.n_workers() == 0); }

    SECTION("empty") { REQUIRE(defaulted.empty()); }

    SECTION("operator==/operator!=") {
        ResourceSet other_defaulted;
        REQUIRE(defaulted == other_defaulted);
        REQUIRE_FALSE(defaulted != other_defaulted);
    }
}
