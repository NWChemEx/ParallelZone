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
#include <parallelzone/logging/detail_/spdlog/spdlog.hpp>
#include <parallelzone/logging/detail_/spdlog/stdout.hpp>
#include <parallelzone/logging/logger.hpp>
#include <spdlog/sinks/ostream_sink.h>
#include <sstream>

using namespace parallelzone;
using pimpl_type = detail_::SpdlogPIMPL;
using severity   = Logger::severity;

/* Testing Strategy:
 *
 * The Logger class has a lot of ways to log the same message. Rather than
 * testing each method individually we test by severity level. Unit tests are
 * set up so that tests at a particular severity level all have the same
 * behavior.
 */

TEST_CASE("Logger") {
    // Make a PIMPL that is easy to test
    std::stringstream ss;
    auto sink       = std::make_shared<spdlog::sinks::ostream_sink_mt>(ss);
    auto spdlog_log = spdlog::logger("ss_log", sink);

    // Remove timestamp to make log statements easier to match
    std::string pattern = "[%n] [%l] %v";
    spdlog_log.set_pattern(pattern);

    Logger log(std::make_unique<pimpl_type>(spdlog_log));
    log.set_severity(severity::trace);

    Logger null;

    SECTION("ctors") {
        SECTION("copy") {
            Logger copy_log(log);
            Logger copy_null(null);
            REQUIRE(copy_log == log);
            REQUIRE(copy_null == null);
        }

        SECTION("move") {
            Logger copy_log(log);
            Logger move_log(std::move(log));
            Logger move_null(std::move(null));
            REQUIRE(move_log == copy_log);
            REQUIRE(move_null == Logger());
        }

        SECTION("copy assignment") {
            auto pnull = &(null = log);
            REQUIRE(pnull == &null);
            REQUIRE(log == null);
        }

        SECTION("move assignment") {
            Logger copy_log(log);
            auto pnull = &(null = std::move(log));
            REQUIRE(pnull == &null);
            REQUIRE(null == copy_log);
        }
    }

    SECTION("set_severity") {
        // The PIMPLs test this more thoroughly. Here we basically make sure
        // arguments get forwarded correctly and that calling the null log
        // doesn't crash
        log.set_severity(severity::debug);
        log.trace("Shouldn't see this");
        log.debug("Should see this");
        log.info("Should also see this");

        std::stringstream corr;
        corr << "[ss_log] [debug] Should see this" << std::endl;
        corr << "[ss_log] [info] Should also see this" << std::endl;
        REQUIRE(ss.str() == corr.str());
        REQUIRE_NOTHROW(null.set_severity(severity::debug));
    }

    SECTION("severity == trace") {
        std::string level("trace");
        std::string msg = "Hello " + level;
        std::stringstream corr;
        corr << "[ss_log] [" << level << "] " << msg << std::endl;

        SECTION(level + " method") {
            auto plog = &(log.trace(msg));
            REQUIRE(plog == &log);
            REQUIRE(ss.str() == corr.str());
            REQUIRE_NOTHROW(null.trace(msg));
        }

        SECTION("log (severity)") {
            auto plog = &(log.log(severity::trace, msg));
            REQUIRE(plog == &log);
            REQUIRE(ss.str() == corr.str());
            REQUIRE_NOTHROW(null.log(severity::trace, msg));
        }
    }

    SECTION("severity == debug") {
        std::string level("debug");
        std::string msg = "Hello " + level;
        std::stringstream corr;
        corr << "[ss_log] [" << level << "] " << msg << std::endl;

        SECTION(level + " method") {
            auto plog = &(log.debug(msg));
            REQUIRE(plog == &log);
            REQUIRE(ss.str() == corr.str());
            REQUIRE_NOTHROW(null.debug(msg));
        }

        SECTION("log (severity)") {
            auto plog = &(log.log(severity::debug, msg));
            REQUIRE(plog == &log);
            REQUIRE(ss.str() == corr.str());
            REQUIRE_NOTHROW(null.log(severity::debug, msg));
        }
    }

    SECTION("severity == info") {
        std::string level("info");
        std::string msg = "Hello " + level;
        std::stringstream corr;
        corr << "[ss_log] [" << level << "] " << msg << std::endl;

        SECTION(level + " method") {
            auto plog = &(log.info(msg));
            REQUIRE(plog == &log);
            REQUIRE(ss.str() == corr.str());
            REQUIRE_NOTHROW(null.info(msg));
        }

        SECTION("log (severity)") {
            auto plog = &(log.log(severity::info, msg));
            REQUIRE(plog == &log);
            REQUIRE(ss.str() == corr.str());
            REQUIRE_NOTHROW(null.log(severity::info, msg));
        }

        SECTION("log (no severity)") {
            auto plog = &(log.log(msg));
            REQUIRE(plog == &log);
            REQUIRE(ss.str() == corr.str());
            REQUIRE_NOTHROW(null.log(msg));
        }

        SECTION("operator<<") {
            auto plog = &(log << msg);
            REQUIRE(plog == &log);
            REQUIRE(ss.str() == corr.str());
            REQUIRE_NOTHROW(null << msg);
        }
    }

    SECTION("severity == warn") {
        std::string level("warning");
        std::string msg = "Hello " + level;
        std::stringstream corr;
        corr << "[ss_log] [" << level << "] " << msg << std::endl;

        SECTION("warn method") {
            auto plog = &(log.warn(msg));
            REQUIRE(plog == &log);
            REQUIRE(ss.str() == corr.str());
            REQUIRE_NOTHROW(null.warn(msg));
        }

        SECTION("log (severity)") {
            auto plog = &(log.log(severity::warn, msg));
            REQUIRE(plog == &log);
            REQUIRE(ss.str() == corr.str());
            REQUIRE_NOTHROW(null.log(severity::warn, msg));
        }
    }

    SECTION("severity == error") {
        std::string level("error");
        std::string msg = "Hello " + level;
        std::stringstream corr;
        corr << "[ss_log] [" << level << "] " << msg << std::endl;

        SECTION(level + " method") {
            auto plog = &(log.error(msg));
            REQUIRE(plog == &log);
            REQUIRE(ss.str() == corr.str());
            REQUIRE_NOTHROW(null.error(msg));
        }

        SECTION("log (severity)") {
            auto plog = &(log.log(severity::error, msg));
            REQUIRE(plog == &log);
            REQUIRE(ss.str() == corr.str());
            REQUIRE_NOTHROW(null.log(severity::error, msg));
        }
    }

    SECTION("severity == critical") {
        std::string level("critical");
        std::string msg = "Hello " + level;
        std::stringstream corr;
        corr << "[ss_log] [" << level << "] " << msg << std::endl;

        SECTION(level + " method") {
            auto plog = &(log.critical(msg));
            REQUIRE(plog == &log);
            REQUIRE(ss.str() == corr.str());
            REQUIRE_NOTHROW(null.critical(msg));
        }

        SECTION("log (severity)") {
            auto plog = &(log.log(severity::critical, msg));
            REQUIRE(plog == &log);
            REQUIRE(ss.str() == corr.str());
            REQUIRE_NOTHROW(null.log(severity::critical, msg));
        }
    }

    SECTION("swap") {
        Logger copy(log);
        log.swap(null);
        REQUIRE(null == copy);
        REQUIRE(log == Logger());
    }

    SECTION("value comparison") {
        // null compared to null
        REQUIRE(null == Logger());
        REQUIRE_FALSE(null != Logger());

        // null compared to non-null
        REQUIRE_FALSE(null == log);
        REQUIRE(null != log);

        // same stateful logger
        REQUIRE(log == Logger(std::make_unique<pimpl_type>(spdlog_log)));
        REQUIRE_FALSE(log != Logger(std::make_unique<pimpl_type>(spdlog_log)));

        // Different state
        auto other_pimpl = detail_::StdoutSpdlog("console");
        Logger other(other_pimpl.clone());
        REQUIRE_FALSE(other == log);
        REQUIRE(other != log);
    }
}
