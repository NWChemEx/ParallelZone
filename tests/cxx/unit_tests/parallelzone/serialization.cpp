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

#include "catch.hpp"
#include "parallelzone/serialization.hpp"

TEMPLATE_TEST_CASE("Serialization with Cereal wrapper",
                   "[serialization][serializer][deserializer]",
                   cereal::BinaryOutputArchive,
                   parallelzone::BinaryOutputArchive,
                   parallelzone::PortableBinaryOutputArchive,
                   parallelzone::JSONOutputArchive,
                   parallelzone::XMLOutputArchive) {
    using output = TestType;
    using input  = typename parallelzone::get_input_from_output<output>::type;
    std::stringstream ss;
    SECTION("Plain-old-data") {
        {
            output ar(ss);
            ar << int{42} << double{3.14} << char{'R'};
        }

        int i;
        double d;
        char c;
        {
            input ar(ss);
            ar >> i >> d >> c;
        }
        REQUIRE(i == int{42});
        REQUIRE(d == double{3.14});
        REQUIRE_FALSE(d == float{3.14f});
        REQUIRE(c == char{'R'});
    }

    SECTION("Containers") {
        std::vector<int> v{1, 2, 3, 4};
        std::map<std::string, double> m{{"Hello", 1.23}, {"World", 3.14}};
        {
            output ar(ss);
            ar << v << m;
        }

        std::vector<int> v2;
        std::map<std::string, double> m2;
        {
            input ar(ss);
            ar >> v2 >> m2;
        }
        REQUIRE(v2 == v);
        REQUIRE(m2 == m);
    }
}
