#include <catch2/catch.hpp>
#include <filesystem>
#include <iostream>
#include <parallelzone/logger/detail_/spdlog.hpp>
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

    SECTION("clone") {
        auto p = ss_log.clone();
        REQUIRE(p->are_equal(ss_log));
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

        // Different sink
        auto sink2     = std::make_shared<spdlog::sinks::ostream_sink_mt>(ss);
        auto sink_temp = spdlog::logger("ss_log", sink2);
        REQUIRE_FALSE(ss_log.are_equal(SpdlogPIMPL(sink_temp)));
    }
}

TEST_CASE("make_stdout_color_mt") {
    // Not really sure how to test this...
    REQUIRE_NOTHROW(make_stdout_color_mt("hello"));
}

TEST_CASE("make_file_mt") {
    // Make sure the file doesn't exist before we do this test. We don't want
    // to mess things up if it does.
    if(!std::filesystem::exists("file1.txt")) {
        auto file = make_file_mt("hello", "file1.txt");
        file.log(severity::info, "hello world");

        REQUIRE(std::filesystem::exists("file1.txt"));
        std::filesystem::remove("file1.txt");
    }
}