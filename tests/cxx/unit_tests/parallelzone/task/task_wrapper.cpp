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

vector_type return_free2(int* pv, vector_type v) {
    REQUIRE(v.data() == pv);
    return v;
}

struct TestFunctor {
    TestFunctor(vector_type v) : m_v(std::move(v)) {}

    vector_type operator()() { return std::move(m_v); }

    vector_type m_v;
};

} // namespace

using type_erased_return_type = TaskWrapper::type_erased_return_type;

TEST_CASE("TaskWrapper") {
    vector_type a_vector{1, 2, 3};
    auto pa_vector = a_vector.data();

    SECTION("Ctor") {
        SECTION("value") {
            // This is heavily tested throughout the rest of the test case.
            // So just spot check we can pass different function types

            SECTION("lambda") {
                auto l = [pa_vector](vector_type v) {
                    REQUIRE(v.data() == pa_vector);
                };
                TaskWrapper t(l, std::move(a_vector));
                t();
            }
            SECTION("free function") {
                TaskWrapper t(no_return_free2, pa_vector, std::move(a_vector));
                t();
            }
            SECTION("Functor") {
                TestFunctor fxn(std::move(a_vector));
                TaskWrapper t(std::move(fxn));
                t();
            }
        }

        SECTION("move") {
            TaskWrapper t(no_return_free2, pa_vector, std::move(a_vector));
            TaskWrapper t_moved(std::move(t));
            t_moved();
        }

        SECTION("move assignment") {
            TaskWrapper t(return_free2, pa_vector, std::move(a_vector));
            TaskWrapper t_moved(no_return_free0);
            auto pt_moved = &(t_moved = std::move(t));
            auto rv       = t_moved();
            REQUIRE(std::any_cast<vector_type&>(rv).data() == pa_vector);
            REQUIRE(pt_moved == &t_moved);
        }
    }

    SECTION("operator()") {
        // This method just runs the wrapped function and returns the
        // result. As long as make_outer_lambda_ works, this method should
        // work and we just spot check
        TaskWrapper t(return_free2, pa_vector, std::move(a_vector));
        auto rv = t();
        REQUIRE(std::any_cast<vector_type&>(rv).data() == pa_vector);
    }

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

        auto task = make_task(no_return_free2, pa_vector, std::move(a_vector));
        std::apply(tester, std::move(task));
    }

    SECTION("return") {
        auto tester = [pa_vector](auto&& task, auto&& unwrapper) {
            type_erased_return_type result;
            REQUIRE_NOTHROW(result = task());
            REQUIRE(result.has_value());
            REQUIRE(unwrapper(std::move(result)).data() == pa_vector);
        };

        auto task = make_task(return_free2, pa_vector, std::move(a_vector));
        std::apply(tester, std::move(task));
    }
}