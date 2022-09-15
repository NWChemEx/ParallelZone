#pragma once
#include <catch2/catch.hpp>
#include <parallelzone/runtime/runtime_view.hpp>

namespace testing {

/** @brief Struct representing the testing environment
 *
 *  At the moment this struct just has the default runtime in it.
 *
 */
struct PZEnvironment {
    static auto& comm_world() { return *pcomm_world; }

    static parallelzone::runtime::RuntimeView* pcomm_world;
};

inline parallelzone::runtime::RuntimeView* PZEnvironment::pcomm_world = nullptr;

} // namespace testing
