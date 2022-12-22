#include <catch2/catch.hpp>
#include <iostream>
#include <parallelzone/logger/detail_/spdlog.hpp>

using namespace parallelzone;
using namespace parallelzone::detail_;
using severity = Logger::severity;

TEST_CASE("map_severity_levels") {
    REQUIRE(map_severity_levels(severity::trace) == spdlog::level::trace);
    REQUIRE(map_severity_levels(severity::debug) == spdlog::level::debug);
    REQUIRE(map_severity_levels(severity::info) == spdlog::level::info);
    REQUIRE(map_severity_levels(severity::warn) == spdlog::level::warn);
    REQUIRE(map_severity_levels(severity::error) == spdlog::level::err);
    REQUIRE(map_severity_levels(severity::critical) == spdlog::level::critical);
}

/* Testing strategy:
 *
 * We know map_severity_levels works. We also assume spdlog works. We basically
 * just need to make sure our PIMPL forwards the severity and message to the
 * underlying logger correctly.
 */
TEST_CASE("SpdlogPIMPL") {
    std::stringstream ss;
    auto sink       = std::make_shared<spdlog::sinks::ostream_sink_mt>(ss);
    auto spdlog_log = std::make_shared<spdlog::logger>("ss_log", sink);

    // Remove timestamp to make log statements easier to match
    spdlog_log->set_pattern("[%n] [%l] %v");
    SpdlogPIMPL ss_log(spdlog_log);

    cout_log.log(severity::trace, "Hello World");
    cout_log.log(severity::debug, "Hello World");
    cout_log.log(severity::info, "Hello World");
    cout_log.log(severity::warn, "Hello World");
    cout_log.log(severity::error, "Hello World");
    cout_log.log(severity::critical, "Hello World");

    std::cout << ss.str() << std::endl;

    SECTION("Ctor") {
        REQUIRE_THROWS_AS(SpdlogPIMPL(nullptr), std::runtime_error);
    }
}