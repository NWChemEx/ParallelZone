#include "../catch.hpp"
#include <iostream>
#include <parallelzone/task/argument_traits.hpp>
#include <vector>

using namespace parallelzone::task;

TEST_CASE("ArgumentTraits<T>") {
    SECTION("value") {
        using type     = int;
        using traits_t = ArgumentTraits<type>;

        STATIC_REQUIRE(std::is_same_v<typename traits_t::value_type, type>);
        STATIC_REQUIRE(std::is_same_v<typename traits_t::reference, type&>);
        STATIC_REQUIRE(
          std::is_same_v<typename traits_t::const_reference, const type&>);
        STATIC_REQUIRE(
          std::is_same_v<typename traits_t::rvalue_reference, type&&>);
        STATIC_REQUIRE(std::is_same_v<typename traits_t::pointer, type*>);
        STATIC_REQUIRE(
          std::is_same_v<typename traits_t::const_pointer, const type*>);
        STATIC_REQUIRE(traits_t::is_value_v);
        STATIC_REQUIRE_FALSE(traits_t::is_const_value_v);
        STATIC_REQUIRE_FALSE(traits_t::is_reference_v);
        STATIC_REQUIRE_FALSE(traits_t::is_const_reference_v);
        STATIC_REQUIRE_FALSE(traits_t::is_rvalue_reference_v);
    }

    SECTION("const value") {
        using type     = int;
        using traits_t = ArgumentTraits<const type>;

        STATIC_REQUIRE(std::is_same_v<typename traits_t::value_type, type>);
        STATIC_REQUIRE(std::is_same_v<typename traits_t::reference, type&>);
        STATIC_REQUIRE(
          std::is_same_v<typename traits_t::const_reference, const type&>);
        STATIC_REQUIRE(
          std::is_same_v<typename traits_t::rvalue_reference, type&&>);
        STATIC_REQUIRE(std::is_same_v<typename traits_t::pointer, type*>);
        STATIC_REQUIRE(
          std::is_same_v<typename traits_t::const_pointer, const type*>);
        STATIC_REQUIRE_FALSE(traits_t::is_value_v);
        STATIC_REQUIRE(traits_t::is_const_value_v);
        STATIC_REQUIRE_FALSE(traits_t::is_reference_v);
        STATIC_REQUIRE_FALSE(traits_t::is_const_reference_v);
        STATIC_REQUIRE_FALSE(traits_t::is_rvalue_reference_v);
    }

    SECTION("reference") {
        using type     = int;
        using traits_t = ArgumentTraits<type&>;

        STATIC_REQUIRE(std::is_same_v<typename traits_t::value_type, type>);
        STATIC_REQUIRE(std::is_same_v<typename traits_t::reference, type&>);
        STATIC_REQUIRE(
          std::is_same_v<typename traits_t::const_reference, const type&>);
        STATIC_REQUIRE(
          std::is_same_v<typename traits_t::rvalue_reference, type&&>);
        STATIC_REQUIRE(std::is_same_v<typename traits_t::pointer, type*>);
        STATIC_REQUIRE(
          std::is_same_v<typename traits_t::const_pointer, const type*>);
        STATIC_REQUIRE_FALSE(traits_t::is_value_v);
        STATIC_REQUIRE_FALSE(traits_t::is_const_value_v);
        STATIC_REQUIRE(traits_t::is_reference_v);
        STATIC_REQUIRE_FALSE(traits_t::is_const_reference_v);
        STATIC_REQUIRE_FALSE(traits_t::is_rvalue_reference_v);
    }

    SECTION("const reference") {
        using type     = int;
        using traits_t = ArgumentTraits<const type&>;

        STATIC_REQUIRE(std::is_same_v<typename traits_t::value_type, type>);
        STATIC_REQUIRE(std::is_same_v<typename traits_t::reference, type&>);
        STATIC_REQUIRE(
          std::is_same_v<typename traits_t::const_reference, const type&>);
        STATIC_REQUIRE(
          std::is_same_v<typename traits_t::rvalue_reference, type&&>);
        STATIC_REQUIRE(std::is_same_v<typename traits_t::pointer, type*>);
        STATIC_REQUIRE(
          std::is_same_v<typename traits_t::const_pointer, const type*>);
        STATIC_REQUIRE_FALSE(traits_t::is_value_v);
        STATIC_REQUIRE_FALSE(traits_t::is_const_value_v);
        STATIC_REQUIRE_FALSE(traits_t::is_reference_v);
        STATIC_REQUIRE(traits_t::is_const_reference_v);
        STATIC_REQUIRE_FALSE(traits_t::is_rvalue_reference_v);
    }

    SECTION("rvalue reference") {
        using type     = int;
        using traits_t = ArgumentTraits<type&&>;

        STATIC_REQUIRE(std::is_same_v<typename traits_t::value_type, type>);
        STATIC_REQUIRE(std::is_same_v<typename traits_t::reference, type&>);
        STATIC_REQUIRE(
          std::is_same_v<typename traits_t::const_reference, const type&>);
        STATIC_REQUIRE(
          std::is_same_v<typename traits_t::rvalue_reference, type&&>);
        STATIC_REQUIRE(std::is_same_v<typename traits_t::pointer, type*>);
        STATIC_REQUIRE(
          std::is_same_v<typename traits_t::const_pointer, const type*>);
        STATIC_REQUIRE_FALSE(traits_t::is_value_v);
        STATIC_REQUIRE_FALSE(traits_t::is_const_value_v);
        STATIC_REQUIRE_FALSE(traits_t::is_reference_v);
        STATIC_REQUIRE_FALSE(traits_t::is_const_reference_v);
        STATIC_REQUIRE(traits_t::is_rvalue_reference_v);
    }
}