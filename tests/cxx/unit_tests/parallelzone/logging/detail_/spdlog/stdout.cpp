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

#include <catch2/catch.hpp>
#include <parallelzone/logging/detail_/spdlog/stdout.hpp>
#include <spdlog/sinks/ostream_sink.h>
#include <sstream>

using namespace parallelzone::detail_;

/* Testing Strategy:
 *
 * Relative to SpdlogPIMPL, this derived class just overrides clone_ and
 * are_equal_ so we just test those.
 */

TEST_CASE("StdoutSpdLog") {
    StdoutSpdlog log0("log 0");
    StdoutSpdlog log1("log 1");

    SECTION("clone") { REQUIRE(log0.clone()->are_equal(log0)); }

    SECTION("are_equal") {
        // Different base
        REQUIRE_FALSE(log0.are_equal(log1));

        // Different derived classes
        std::stringstream ss;
        auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(ss);
        SpdlogPIMPL other(spdlog::logger("ss_log", sink));
        REQUIRE_FALSE(log0.are_equal(other));
        REQUIRE_FALSE(log1.are_equal(other));
    }
}
