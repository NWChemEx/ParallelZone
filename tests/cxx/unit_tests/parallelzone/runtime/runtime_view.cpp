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

#include "../test_parallelzone.hpp"
#include <iostream>
#include <parallelzone/logging/logger_factory.hpp>
#include <parallelzone/mpi_helpers/commpp/commpp.hpp>
#include <parallelzone/runtime/detail_/resource_set_pimpl.hpp>
#include <sstream>

using namespace parallelzone;
using namespace runtime;

/* Testing notes
 *
 * Here we have similar caveats to the RuntimeViewPIMPL unit tests, namely we
 * need to avoid restarting and/or shutting down CommPP (and MPI under it). By
 * time we get into this test the main function will have initialized CommPP by
 * calling the RuntimeView(argc_type, argv_type) ctor.
 *
 * These unit tests assume MPI_COMM_WORLD is the top-level communicator given
 * to the unit test executable.
 */

TEST_CASE("RuntimeView") {
    using pimpl_pointer = RuntimeView::pimpl_pointer;
    using logger_type   = RuntimeView::logger_type;

    RuntimeView null(pimpl_pointer{});
    RuntimeView argc_argv = testing::PZEnvironment::comm_world();
    RuntimeView defaulted;

    mpi_helpers::CommPP comm(defaulted.mpi_comm());

    SECTION("CTors") {
        SECTION("Default") {
            REQUIRE(defaulted.size() > 0);
            REQUIRE(defaulted.mpi_comm() == MPI_COMM_WORLD);
            REQUIRE_FALSE(defaulted.did_i_start_mpi());
        }
        SECTION("argc and argv") {
            REQUIRE(argc_argv.size() > 0);
            REQUIRE(argc_argv.mpi_comm() == MPI_COMM_WORLD);
            REQUIRE(argc_argv.did_i_start_mpi());
        }
        SECTION("mpi comm") {
            RuntimeView mpi_comm(argc_argv.mpi_comm());
            REQUIRE(mpi_comm == argc_argv);
        }
        SECTION("primary") {
            RuntimeView primary(0, nullptr, argc_argv.mpi_comm());
            REQUIRE(primary == argc_argv);
        }
        SECTION("state") {
            REQUIRE(null.size() == 0);
            REQUIRE(null.mpi_comm() == MPI_COMM_NULL);
        }
        SECTION("copy") {
            RuntimeView null_copy(null);
            RuntimeView defaulted_copy(defaulted);
            RuntimeView argc_argv_copy(argc_argv);

            REQUIRE(null_copy == null);
            REQUIRE(defaulted_copy == defaulted);
            REQUIRE(argc_argv_copy == argc_argv);
        }
        SECTION("copy assignment") {
            RuntimeView null_copy;
            auto pnull_copy = &(null_copy = null);

            RuntimeView defaulted_copy;
            auto pdefaulted_copy = &(defaulted_copy = defaulted);

            RuntimeView argc_argv_copy;
            auto pargc_argv_copy = &(argc_argv_copy = argc_argv);

            REQUIRE(pnull_copy == &null_copy);
            REQUIRE(pdefaulted_copy == &defaulted_copy);
            REQUIRE(pargc_argv_copy == &argc_argv_copy);

            REQUIRE(null_copy == null);
            REQUIRE(defaulted_copy == defaulted);
            REQUIRE(argc_argv_copy == argc_argv);
        }
        SECTION("move") {
            RuntimeView null_copy(null);
            RuntimeView null_move(std::move(null));

            RuntimeView defaulted_copy(defaulted);
            RuntimeView defaulted_move(std::move(defaulted));

            RuntimeView argc_argv_copy(argc_argv);
            RuntimeView argc_argv_move(std::move(argc_argv));

            REQUIRE(null_copy == null_move);
            REQUIRE(defaulted_copy == defaulted_move);
            REQUIRE(argc_argv_copy == argc_argv_move);
        }
        SECTION("move assignment") {
            RuntimeView null_copy(null);
            RuntimeView null_move;
            auto pnull_move = &(null_move = std::move(null));

            RuntimeView defaulted_copy(defaulted);
            RuntimeView defaulted_move;
            auto pdefaulted_move = &(defaulted_move = std::move(defaulted));

            RuntimeView argc_argv_copy(argc_argv);
            RuntimeView argc_argv_move;
            auto pargc_argv_move = &(argc_argv_move = std::move(argc_argv));

            REQUIRE(pnull_move == &null_move);
            REQUIRE(pdefaulted_move == &defaulted_move);
            REQUIRE(pargc_argv_move == &argc_argv_move);

            REQUIRE(null_copy == null_move);
            REQUIRE(defaulted_copy == defaulted_move);
            REQUIRE(argc_argv_copy == argc_argv_move);
        }
    }

    SECTION("mpi_comm") {
        int result;

        // N.B. AFAIK in Open MPI, MPI_Comm_compare won't work for MPI_COMM_NULL
        auto comm = null.mpi_comm();
        REQUIRE(comm == MPI_COMM_NULL);

        comm = defaulted.mpi_comm();
        MPI_Comm_compare(comm, MPI_COMM_WORLD, &result);
        REQUIRE(result == MPI_IDENT);

        comm = argc_argv.mpi_comm();
        MPI_Comm_compare(comm, MPI_COMM_WORLD, &result);
        REQUIRE(result == MPI_IDENT);
    }

    SECTION("size()") {
        REQUIRE(null.size() == 0);
        REQUIRE(defaulted.size() > 0);
        REQUIRE(argc_argv.size() > 0);
    }

    SECTION("did_i_start_mpi") {
        REQUIRE_FALSE(null.did_i_start_mpi());
        REQUIRE_FALSE(defaulted.did_i_start_mpi());
        REQUIRE(argc_argv.did_i_start_mpi());
    }

    SECTION("at()") {
        REQUIRE_THROWS_AS(null.at(0), std::out_of_range);
        auto n_resource_sets = defaulted.size();
        logger_type log;
        for(decltype(n_resource_sets) i = 0; i < n_resource_sets; ++i) {
            auto corr = runtime::detail_::make_resource_set(i, comm, log);
            REQUIRE(defaulted.at(i) == corr);
            REQUIRE(argc_argv.at(i) == corr);
        }
        REQUIRE_THROWS_AS(defaulted.at(n_resource_sets), std::out_of_range);
    }

    SECTION("has_me()") {
        REQUIRE_FALSE(null.has_me());
        REQUIRE(defaulted.has_me());
        REQUIRE(argc_argv.has_me());
    }

    SECTION("my_resource_set") {
        REQUIRE_THROWS_AS(null.my_resource_set(), std::runtime_error);
        logger_type log;
        auto corr = runtime::detail_::make_resource_set(comm.me(), comm, log);
        REQUIRE(defaulted.my_resource_set() == corr);
        REQUIRE(argc_argv.my_resource_set() == corr);
    }

    SECTION("count(RAM)") {
        RuntimeView::ram_type ram;
        auto rank0_ram = defaulted.at(0).ram();

        REQUIRE(null.count(ram) == 0);
        REQUIRE(null.count(rank0_ram) == 0);

        REQUIRE(defaulted.count(ram) == 0);
        REQUIRE(defaulted.count(rank0_ram) >= 1);

        REQUIRE(argc_argv.count(ram) == 0);
        REQUIRE(argc_argv.count(rank0_ram) >= 1);
    }

    SECTION("logger") {
        if(defaulted.my_resource_set().mpi_rank() != 0) {
            REQUIRE(defaulted.logger() == Logger());
        } else {
            REQUIRE(defaulted.logger() != Logger());
        }
    }

    SECTION("stack_callback I") {
        // Simulate initialization
        bool is_running = true;

        // Simulate finalize callback
        auto turn_off = [&is_running]() { is_running = false; };

        // RuntimeView will fall off, call the turn_off lambda
        {
            RuntimeView falls_off;
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

        // RuntimeView will fall off, call the turn_off lambda
        {
            RuntimeView rt;
            rt.stack_callback(call_back_1);
            rt.stack_callback(call_back_2);
        }

        REQUIRE(func_no == 3);
    }

    SECTION("gather") {
        using data_type = std::vector<std::string>;
        data_type local_data(3, "Hello");
        auto rv = defaulted.gather(local_data);
        std::vector<data_type> corr(defaulted.size(), local_data);
        REQUIRE(rv == corr);
    }

    SECTION("gatherv") {
        using data_type = std::vector<std::string>;
        data_type local_data(3, "Hello");
        auto rv = defaulted.gatherv(local_data);
        std::vector<data_type> corr(defaulted.size(), local_data);
        REQUIRE(rv == corr);
    }

    SECTION("reduce") {
        using data_type = std::vector<double>;
        data_type local_data(3, 1.0);
        auto rv = defaulted.reduce(local_data, std::plus<double>());
        data_type corr(3, comm.size());
        REQUIRE(rv == corr);
    }

    SECTION("swap") {
        RuntimeView defaulted_copy(defaulted);
        RuntimeView argc_argv_copy(argc_argv);

        defaulted.swap(argc_argv);
        REQUIRE(defaulted == argc_argv_copy);
        REQUIRE(argc_argv == defaulted_copy);
    }

    SECTION("operator==/operator!=") {
        // Different communicators
        REQUIRE(defaulted != null);
        REQUIRE_FALSE(defaulted == null);

        // Different loggers
        bool am_i_0 = argc_argv.at(0).is_mine();

        // This assumes that rank 0 has a different logger than the other ranks
        // and sets rank 0's logger to that of rank 1, and all other ranks get
        // rank 0's logger
        defaulted.logger() = LoggerFactory::default_global_logger(am_i_0);
        REQUIRE(defaulted != argc_argv);
        REQUIRE_FALSE(defaulted == argc_argv);
    }
}
