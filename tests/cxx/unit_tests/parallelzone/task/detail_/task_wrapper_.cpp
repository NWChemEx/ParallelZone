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

#include "../../catch.hpp"
#include <iostream>
#include <parallelzone/task/detail_/task_wrapper_.hpp>
#include <utility>
#include <vector>

using namespace parallelzone::task;

namespace {

using vector_type = std::vector<int>;

} // namespace

TEST_CASE("wrap_args_") {
    int an_int = 1;
    vector_type a_vector{1, 2, 3};
    auto pa_vector = a_vector.data();

    SECTION("No args") {
        auto rv = detail_::wrap_args_();
        REQUIRE(rv == std::make_tuple());
    }

    SECTION("by value") {
        auto int_rv = detail_::wrap_args_(1);
        REQUIRE(std::get<0>(int_rv).value() == 1);

        auto vector_rv = detail_::wrap_args_(vector_type{1, 2, 3});
        REQUIRE(std::get<0>(vector_rv).value() == a_vector);
        REQUIRE(std::get<0>(vector_rv).value().data() != pa_vector);
    }

    SECTION("by reference") {
        auto int_rv = detail_::wrap_args_(an_int);
        REQUIRE(std::get<0>(int_rv).value() == 1);
        REQUIRE(&std::get<0>(int_rv).value() == &an_int);

        auto vector_rv = detail_::wrap_args_(a_vector);
        REQUIRE(std::get<0>(vector_rv).value() == a_vector);
        REQUIRE(&std::get<0>(vector_rv).value() == &a_vector);
    }

    SECTION("by const reference") {
        auto int_rv = detail_::wrap_args_(std::as_const(an_int));
        REQUIRE(std::get<0>(int_rv).value() == 1);
        REQUIRE(&std::get<0>(int_rv).value() == &an_int);

        auto vector_rv = detail_::wrap_args_(std::as_const(a_vector));
        REQUIRE(std::get<0>(vector_rv).value() == a_vector);
        REQUIRE(&std::get<0>(vector_rv).value() == &a_vector);
    }

    SECTION("by rvalue reference") {
        auto vector_rv = detail_::wrap_args_(std::move(a_vector));
        REQUIRE(std::get<0>(vector_rv).value().data() == pa_vector);
    }
}

TEST_CASE("apply_") {
    vector_type a_vector{1, 2, 3};
    auto pa_vector = a_vector.data();

    SECTION("Function takes no arguments") {
        auto inputs = detail_::wrap_args_();
        auto lambda = []() {};
        REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    }

    SECTION("Function takes by value and passed by value") {
        auto inputs = detail_::wrap_args_(vector_type{1, 2, 3});
        auto lambda = [&a_vector](vector_type x) { REQUIRE(x == a_vector); };
        REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    }

    SECTION("Function takes by value and passed by reference") {
        auto inputs = detail_::wrap_args_(a_vector);
        auto lambda = [&a_vector](vector_type x) {
            REQUIRE(x == a_vector);
            REQUIRE(x.data() != a_vector.data());
        };
        REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    }

    SECTION("Function takes by value and passed by const reference") {
        auto inputs = detail_::wrap_args_(std::as_const(a_vector));
        auto lambda = [&a_vector](vector_type x) {
            REQUIRE(x == a_vector);
            REQUIRE(x.data() != a_vector.data());
        };
        REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    }

    SECTION("Function takes by value and passed by rvalue") {
        auto inputs = detail_::wrap_args_(std::move(a_vector));
        auto lambda = [pa_vector](vector_type x) {
            REQUIRE(x.data() == pa_vector);
        };
        REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    }

    // Should raise a compiler error
    // SECTION("Function takes by reference and passed by value") {
    //    auto inputs = detail_::wrap_args_(vector_type{1, 2, 3});
    //    auto lambda = [&a_vector](vector_type& x) { };
    //    REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    //}

    SECTION("Function takes by reference and passed by reference") {
        auto inputs = detail_::wrap_args_(a_vector);
        auto lambda = [&a_vector](vector_type& x) { REQUIRE(&x == &a_vector); };
        REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    }

    // Should raise a compiler error
    // SECTION("Function takes by reference and passed by const reference") {
    //     auto inputs = detail_::wrap_args_(std::as_const(a_vector));
    //     auto lambda = [&a_vector](vector_type& x) { };
    //     REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    // }

    // Should raise a compiler error
    // SECTION("Function takes by reference and passed by rvalue reference") {
    //     auto inputs = detail_::wrap_args_(std::move(a_vector));
    //    auto lambda = [pa_vector](vector_type& x) { };
    //     REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    // }

    SECTION("Function takes by const reference and passed by value") {
        auto inputs = detail_::wrap_args_(vector_type{1, 2, 3});
        auto lambda = [&a_vector](const vector_type& x) {
            REQUIRE(x == a_vector);
        };
        REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    }

    SECTION("Function takes by const reference and passed by reference") {
        auto inputs = detail_::wrap_args_(a_vector);
        auto lambda = [&a_vector](const vector_type& x) {
            REQUIRE(&x == &a_vector);
        };
        REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    }

    SECTION("Function takes by const reference and passed by const reference") {
        auto inputs = detail_::wrap_args_(std::as_const(a_vector));
        auto lambda = [&a_vector](const vector_type& x) {
            REQUIRE(&x == &a_vector);
        };
        REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    }

    SECTION(
      "Function takes by const reference and passed by rvalue reference") {
        auto inputs = detail_::wrap_args_(std::move(a_vector));
        auto lambda = [pa_vector](const vector_type& x) {
            REQUIRE(x.data() == pa_vector);
        };
        REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    }

    SECTION("Function takes by rvalue reference and passed by value") {
        auto inputs = detail_::wrap_args_(vector_type{1, 2, 3});
        auto lambda = [&a_vector](vector_type&& x) { REQUIRE(x == a_vector); };
        REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    }

    // Should raise a compiler error
    // SECTION("Function takes by rvalue reference and passed by reference") {
    //     auto inputs = detail_::wrap_args_(a_vector);
    //     auto lambda = [&a_vector](vector_type&& x) {};
    //     REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    // }

    // Should raise a compiler error
    // SECTION(
    //   "Function takes by rvalue reference and passed by const reference") {
    //     auto inputs = detail_::wrap_args_(std::as_const(a_vector));
    //     auto lambda = [&a_vector](vector_type&& x) {};
    //     REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    // }

    SECTION(
      "Function takes by rvalue reference and passed by rvalue reference") {
        auto inputs = detail_::wrap_args_(std::move(a_vector));
        auto lambda = [pa_vector](vector_type&& x) {
            REQUIRE(x.data() == pa_vector);
        };
        REQUIRE_NOTHROW(detail_::apply_(lambda, std::move(inputs)));
    }

    SECTION("Function returns by value") {
        auto inputs = detail_::wrap_args_(std::move(a_vector));
        auto lambda = [pa_vector](vector_type x) {
            REQUIRE(x.data() == pa_vector);
            return x;
        };
        auto rv = detail_::apply_(lambda, std::move(inputs));
        REQUIRE(rv.data() == pa_vector);
    }
}

TEST_CASE("make_inner_lambda_") {
    // This function is largely implemented by wrap_args_ and apply_. As long
    // as those two functions work this function should work. Point being we
    // just spot check a round-trip of the input

    vector_type a_vector{1, 2, 3};
    auto pa_vector = a_vector.data();

    auto lambda = [pa_vector](vector_type x) {
        REQUIRE(x.data() == pa_vector);
        return x;
    };
    auto lambda2 = detail_::make_inner_lambda_(lambda, std::move(a_vector));
    REQUIRE(lambda2().data() == pa_vector);
}

TEST_CASE("make_outer_lambda_") {
    // This function is largely implemented by make_inner_lambda_. Here we just
    // check it wraps the return correctly

    vector_type a_vector{1, 2, 3};
    auto pa_vector = a_vector.data();

    SECTION("Doesn't return") {
        auto lambda = [pa_vector](vector_type x) {
            REQUIRE(x.data() == pa_vector);
        };
        auto lambda2 = detail_::make_outer_lambda_(lambda, std::move(a_vector));
        REQUIRE_FALSE(lambda2().has_value());
    }

    SECTION("Returns") {
        auto lambda = [pa_vector](vector_type x) {
            REQUIRE(x.data() == pa_vector);
            return x;
        };
        auto lambda2 = detail_::make_outer_lambda_(lambda, std::move(a_vector));
        REQUIRE(std::any_cast<vector_type&&>(lambda2()).data() == pa_vector);
    }
}
