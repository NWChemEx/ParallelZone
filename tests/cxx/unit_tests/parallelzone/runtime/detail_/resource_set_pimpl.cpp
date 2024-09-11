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

#include "../../catch.hpp"
#include <parallelzone/logging/logger_factory.hpp>
#include <parallelzone/runtime/detail_/resource_set_pimpl.hpp>

using namespace parallelzone::runtime;
using namespace parallelzone::runtime::detail_;

TEST_CASE("ResourceSetPIMPL") {
    // N.B. At this level in the hierarchy there's no check for whether the
    //      comm actually has more than one rank so these tests should work
    //      regardless of many ranks are used for testing

    using comm_type = ResourceSetPIMPL::mpi_comm_type;
    comm_type comm(MPI_COMM_WORLD);

    parallelzone::Logger log;

    ResourceSetPIMPL rank0(0, comm, log);
    ResourceSetPIMPL rank1(1, comm, log);

    SECTION("CTor") {
        REQUIRE(rank0.m_rank == 0);
        REQUIRE(rank1.m_rank == 1);

        using parallelzone::hardware::detail_::make_ram;
        REQUIRE(rank0.m_ram == make_ram(get_ram_size(), 0, rank0.m_my_mpi));
        REQUIRE(rank1.m_ram == make_ram(get_ram_size(), 1, rank1.m_my_mpi));

        REQUIRE(rank0.m_my_mpi == comm);
        REQUIRE(rank1.m_my_mpi == comm);

        REQUIRE(*rank0.m_plogger == log);
        REQUIRE(*rank1.m_plogger == log);
    }

    SECTION("clone") {
        REQUIRE(*rank0.clone() == rank0);
        REQUIRE(*rank1.clone() == rank1);
    }

    SECTION("operator==") {
        // Same
        REQUIRE(rank0 == ResourceSetPIMPL(0, comm, log));
        REQUIRE(rank1 == ResourceSetPIMPL(1, comm, log));

        // Different ranks
        REQUIRE_FALSE(rank0 == rank1);

        // Different MPI communicators
        REQUIRE_FALSE(rank0 == ResourceSetPIMPL(0, comm_type{}, log));
        REQUIRE_FALSE(rank1 == ResourceSetPIMPL(1, comm_type{}, log));

        // Different loggers (N.B. rank0 and rank1 contain null logger, not
        // the loggers they would have if correctly setup)
        auto logger0 = parallelzone::LoggerFactory::default_global_logger(0);
        REQUIRE_FALSE(rank0 == ResourceSetPIMPL(0, comm, logger0));
        REQUIRE_FALSE(rank1 == ResourceSetPIMPL(1, comm, logger0));
    }
}

TEST_CASE("get_ram_size") { REQUIRE(get_ram_size() == 10); }

TEST_CASE("make_resource_set") {
    using comm_type = ResourceSetPIMPL::mpi_comm_type;
    comm_type comm(MPI_COMM_WORLD);
    parallelzone::Logger log;
    auto p = std::make_unique<ResourceSetPIMPL>(0, comm, log);
    REQUIRE(make_resource_set(0, comm, log) == ResourceSet(std::move(p)));
}
