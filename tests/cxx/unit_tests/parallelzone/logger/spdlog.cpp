#include <catch2/catch.hpp>
#include <iostream>
#include <parallelzone/logger/detail_/spdlog.hpp>

using namespace parallelzone::detail_;

TEST_CASE("SpdlogPIMPL") {
    std::stringstream ss;
    auto cout_log = make_sstream_mt("ss", ss);
    cout_log.trace("Hello World");
    cout_log.debug("Hello World");
    cout_log.info("Hello World");
    cout_log.warn("Hello World");
    cout_log.error("Hello World");
    cout_log.critical("Hello World");

    std::cout << ss.str() << std::endl;
}