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

#include "../../test_parallelzone.hpp"
#include <parallelzone/logging/logger_factory.hpp>
#include <parallelzone/runtime/detail_/runtime_view_pimpl.hpp>

using namespace parallelzone::runtime;
using namespace parallelzone::runtime::detail_;

/* Testing Notes
 *
 * Unfortunately fully testing the ctor/dtor is tricky because we can't restart
 * MPI. In particular this means if we make a
 * RuntimeViewPIMPL instance and tell it that it started MPI, when it goes
 * out of scope it will finalize MPI and break all of the other tests in
 * ParallelZone (unless it just so happens that this is the only test run or
 * the last one).
 *
 * At this point the logger is mainly just along for the ride, so we use null
 * loggers for most tests.
 */

TEST_CASE("RuntimeViewPIMPL") {
    auto& rt = testing::PZEnvironment::comm_world();
    RuntimeViewPIMPL::comm_type comm(rt.mpi_comm());
    RuntimeViewPIMPL::comm_type null_comm;
    parallelzone::Logger log;
    RuntimeViewPIMPL pimpl(false, comm, log);

    SECTION("CTor") {
        REQUIRE_FALSE(pimpl.m_did_i_start_mpi);
        REQUIRE(pimpl.m_comm == comm);
    }

    SECTION("at") {
        REQUIRE(pimpl.at(0) == make_resource_set(0, comm, log));

        if(comm.size() > 1) {
            REQUIRE(pimpl.at(1) == make_resource_set(1, comm, log));
        }
    }

    SECTION("operator==") {
        SECTION("Same") {
            RuntimeViewPIMPL other(false, comm, log);
            REQUIRE(pimpl == other);
        }

        SECTION("Different communicator") {
            RuntimeViewPIMPL other(false, comm, log);
            other.m_comm = null_comm;
            REQUIRE_FALSE(pimpl == other);
        }

        SECTION("Different logger") {
            // This assumes the default logger for rank 0 isn't a null logger
            auto log0 = parallelzone::LoggerFactory::default_global_logger(0);
            RuntimeViewPIMPL other(false, comm, log0);
            REQUIRE_FALSE(pimpl == other);
        }
    }

    SECTION("stack_callback I") {
        // Simulate initialization
        bool is_running = true;

        // Simulate finalize callback
        auto turn_off = [&is_running]() { is_running = false; };

        // RuntimeViewPIMPL will fall off, call the turn_off lambda
        {
            RuntimeViewPIMPL falls_off(false, comm, log);
            falls_off.stack_callback(turn_off);
        }
        REQUIRE(is_running == false);
    }

    SECTION("stack_callback II") {
        // Testing the stack
        int func_no = 1;

        // Two lambdas to be pushed into the stack
        auto call_back_1 = [&func_no]() { func_no += 1; };
        auto call_back_2 = [&func_no]() { func_no *= 2; };

        // RuntimeViewPIMPL will fall off, call the turn_off lambda
        {
            RuntimeViewPIMPL rt_pimpl(false, comm, log);
            rt_pimpl.stack_callback(call_back_1);
            rt_pimpl.stack_callback(call_back_2);
        }

        REQUIRE(func_no == 3);
    }
}
