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
