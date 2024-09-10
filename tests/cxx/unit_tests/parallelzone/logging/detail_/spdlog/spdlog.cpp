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

#include "../../../catch.hpp"
#include <filesystem>
#include <iostream>
#include <parallelzone/logging/detail_/spdlog/spdlog.hpp>
#include <spdlog/sinks/ostream_sink.h>
#include <sstream>

using namespace parallelzone;
using namespace parallelzone::detail_;
using severity = Logger::severity;

/* Testing strategy:
 *
 * We assume spdlog works, so we basically just need to make sure our PIMPL
 * forwards the severity and message to the underlying logger correctly,
 * nonetheless, for good measure we push back one message of each severity.
 */
TEST_CASE("SpdlogPIMPL") {
    std::stringstream ss;
    auto sink       = std::make_shared<spdlog::sinks::ostream_sink_mt>(ss);
    auto spdlog_log = spdlog::logger("ss_log", sink);

    // Remove timestamp to make log statements easier to match
    std::string pattern = "[%n] [%l] %v";
    spdlog_log.set_pattern(pattern);
    SpdlogPIMPL ss_log(spdlog_log);
    ss_log.set_severity(severity::trace);

    SECTION("clone") {
        auto p = ss_log.clone();
        REQUIRE(p->are_equal(ss_log));
    }

    SECTION("set_severity") {
        std::vector<severity> all_levels{severity::trace, severity::debug,
                                         severity::info,  severity::warn,
                                         severity::error, severity::critical};

        for(decltype(all_levels.size()) i = 0; i < all_levels.size(); ++i) {
            SECTION("level = " + std::to_string(i)) {
                ss_log.set_severity(all_levels[i]);
                ss_log.log(severity::trace, "Hello trace");
                ss_log.log(severity::debug, "Hello debug");
                ss_log.log(severity::info, "Hello info");
                ss_log.log(severity::warn, "Hello warn");
                ss_log.log(severity::error, "Hello error");
                ss_log.log(severity::critical, "Hello critical");

                std::stringstream corr;
                if(i < 1) corr << "[ss_log] [trace] Hello trace" << std::endl;
                if(i < 2) corr << "[ss_log] [debug] Hello debug" << std::endl;
                if(i < 3) corr << "[ss_log] [info] Hello info" << std::endl;
                if(i < 4) corr << "[ss_log] [warning] Hello warn" << std::endl;
                if(i < 5) corr << "[ss_log] [error] Hello error" << std::endl;
                corr << "[ss_log] [critical] Hello critical" << std::endl;
                REQUIRE(corr.str() == ss.str());
            }
        }
    }

    SECTION("log") {
        ss_log.log(severity::trace, "Hello trace");
        ss_log.log(severity::debug, "Hello debug");
        ss_log.log(severity::info, "Hello info");
        ss_log.log(severity::warn, "Hello warn");
        ss_log.log(severity::error, "Hello error");
        ss_log.log(severity::critical, "Hello critical");

        std::stringstream corr;
        corr << "[ss_log] [trace] Hello trace" << std::endl;
        corr << "[ss_log] [debug] Hello debug" << std::endl;
        corr << "[ss_log] [info] Hello info" << std::endl;
        corr << "[ss_log] [warning] Hello warn" << std::endl;
        corr << "[ss_log] [error] Hello error" << std::endl;
        corr << "[ss_log] [critical] Hello critical" << std::endl;
        REQUIRE(corr.str() == ss.str());
    }

    SECTION("are_equal") {
        // Same (pattern isn't compared)
        auto same = spdlog::logger("ss_log", sink);
        REQUIRE(ss_log.are_equal(SpdlogPIMPL(same)));

        // See comment in SpdlogPIMPL::are_equal_ implementation for why
        // this is commented out.

        // Different levels
        // auto level_temp = spdlog::logger("ss_log", sink);
        // level_temp.set_level(spdlog::level::warn);
        // REQUIRE_FALSE(ss_log.are_equal(SpdlogPIMPL(level_temp)));

        // Different name
        auto name = SpdlogPIMPL(spdlog::logger("ss_log2", sink));
        REQUIRE_FALSE(ss_log.are_equal(name));
    }
}
