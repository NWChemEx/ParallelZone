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
#include <parallelzone/task/task_wrapper.hpp>
#include <vector>

using namespace parallelzone::task;

namespace {

using vector_type = std::vector<int>;

// Free functions with no returns and various inputs
void no_return_free0() {}
void no_return_free1(int) {}
void no_return_free2(int* pv, vector_type v) { REQUIRE(v.data() == pv); }

} // namespace

using type_erased_return_type = TaskWrapper::type_erased_return_type;

TEST_CASE("TaskWrapper") {
    vector_type a_vector{1, 2, 3};
    auto pa_vector = a_vector.data();

    // SECTION("Constructor") {
    //     TaskWrapper t(no_return_free2, pa_vector, std::move(a_vector));
    //     // t();
    // }

    SECTION("unwrapper") {
        SECTION("no result, i.e., void") {
            auto u = TaskWrapper::unwrapper<void>();
            std::any empty;
            REQUIRE_NOTHROW(u(std::move(empty)));
        }

        SECTION("int") {
            auto u      = TaskWrapper::unwrapper<int>();
            auto da_any = std::make_any<int>(1);
            REQUIRE(u(std::move(da_any)) == 1);
        }

        SECTION("vector") {
            auto u      = TaskWrapper::unwrapper<vector_type>();
            auto da_any = std::make_any<vector_type>(std::move(a_vector));
            REQUIRE(std::any_cast<vector_type&>(da_any).data() == pa_vector);
            REQUIRE(u(std::move(da_any)).data() == pa_vector);
        }
    }

    SECTION("No return") {
        auto tester = [](auto&& task, auto&& unwrapper) {
            type_erased_return_type result;
            REQUIRE_NOTHROW(result = task());
            REQUIRE_FALSE(result.has_value());
            REQUIRE_NOTHROW(unwrapper(std::move(result)));
        };

        std::apply(tester, make_task(no_return_free0));
        std::apply(tester, make_task(no_return_free1, 1));

        // This one makes sure that a moved object is forwarded all the way
        // into the task.
        auto task = make_task(no_return_free2, pa_vector, std::move(a_vector));
        // std::get<0>(task)();
        //  std::apply(tester, std::move(task));
    }
}