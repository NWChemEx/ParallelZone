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

#include "test_traits.hpp"
#include <parallelzone/mpi_helpers/traits/gather.hpp>

// N.B. These tests rely on the type lists in test_traits.hpp being up to date
using namespace parallelzone::mpi_helpers;

TEMPLATE_LIST_TEST_CASE("gather_return_t(no serialization)", "",
                        testing::false_list) {
    using T = TestType;
    STATIC_REQUIRE(std::is_same_v<gather_return_t<T>, std::optional<T>>);
}

TEMPLATE_LIST_TEST_CASE("gather_return_t(needs serialized)", "",
                        testing::true_list) {
    using T    = TestType;
    using corr = std::optional<std::vector<T>>;
    STATIC_REQUIRE(std::is_same_v<gather_return_t<T>, corr>);
}
