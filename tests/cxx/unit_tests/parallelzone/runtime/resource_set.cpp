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
#include <parallelzone/runtime/detail_/resource_set_pimpl.hpp>
#include <parallelzone/runtime/resource_set.hpp>

using namespace parallelzone::runtime;

/* Testing Notes:
 *
 * In practice ResourceSets are either null or affiliated with a RuntimeView.
 * For testing a non-null ResourceSet we grab the ResourceSet for the
 * local process from the RuntimeView. If this test is run under MPI that
 * means that `rs` will have different states on each process.
 *
 */
TEST_CASE("ResourceSet") {
    using pimpl_type  = ResourceSet::pimpl_type;
    using comm_type   = parallelzone::mpi_helpers::CommPP;
    using logger_type = ResourceSet::logger_type;
    using size_type   = ResourceSet::size_type;
    const size_type null_rank(MPI_PROC_NULL);

    logger_type log;

    auto& world = testing::PZEnvironment::comm_world();
    comm_type comm(world.mpi_comm());

    // Null b/c defaulted
    ResourceSet defaulted;

    // Null b/c PIMPL was set to null
    ResourceSet null(std::make_unique<pimpl_type>(null_rank, comm_type{}, log));

    // Not null or empty
    auto& rs = world.my_resource_set();

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            REQUIRE(defaulted.mpi_rank() == null_rank);
            REQUIRE_FALSE(defaulted.is_mine());
            REQUIRE_FALSE(defaulted.has_ram());
        }

        SECTION("Value") {
            REQUIRE(null.mpi_rank() == null_rank);
            REQUIRE_FALSE(null.is_mine());
            REQUIRE_FALSE(null.has_ram());

            REQUIRE(rs.mpi_rank() == size_type(comm.me()));
            REQUIRE(rs.is_mine());
            REQUIRE(rs.has_ram()); // I presume all computers have RAM...
        }

        SECTION("Copy") {
            ResourceSet defaulted_copy(defaulted);
            REQUIRE(defaulted_copy == defaulted);

            ResourceSet rs_copy(rs);
            REQUIRE(rs_copy == rs);
        }

        SECTION("Move") {
            ResourceSet defaulted_copy(defaulted);
            ResourceSet defaulted_move(std::move(defaulted));
            REQUIRE(defaulted_copy == defaulted_move);

            ResourceSet rs_copy(rs);
            ResourceSet rs_move(std::move(rs));
            REQUIRE(rs_copy == rs_move);
        }

        SECTION("Copy Assignment") {
            ResourceSet defaulted_copy;
            auto pdefaulted_copy = &(defaulted_copy = defaulted);
            REQUIRE(pdefaulted_copy == &defaulted_copy);
            REQUIRE(defaulted_copy == defaulted);

            ResourceSet rs_copy;
            auto prs_copy = &(rs_copy = rs);
            REQUIRE(prs_copy == &rs_copy);
            REQUIRE(rs_copy == rs);
        }

        SECTION("Move Assignment") {
            ResourceSet defaulted_copy(defaulted);
            ResourceSet defaulted_move;
            auto pdefaulted_move = &(defaulted_move = std::move(defaulted));
            REQUIRE(pdefaulted_move == &defaulted_move);
            REQUIRE(defaulted_copy == defaulted_move);

            ResourceSet rs_copy(rs);
            ResourceSet rs_move;
            auto prs_move = &(rs_move = std::move(rs));
            REQUIRE(prs_move == &rs_move);
            REQUIRE(rs_copy == rs_move);
        }
    }

    SECTION("mpi_rank") {
        REQUIRE(defaulted.mpi_rank() == null_rank);
        REQUIRE(null.mpi_rank() == null_rank);
        REQUIRE(rs.mpi_rank() == size_type(comm.me()));
    }

    SECTION("is_mine") {
        REQUIRE_FALSE(defaulted.is_mine());
        REQUIRE_FALSE(null.is_mine());
        REQUIRE(rs.is_mine());
    }

    SECTION("has_ram") {
        REQUIRE_FALSE(defaulted.has_ram());
        REQUIRE_FALSE(null.has_ram());
        REQUIRE(rs.has_ram());
    }

    SECTION("ram") {
        REQUIRE_THROWS_AS(defaulted.ram(), std::out_of_range);
        REQUIRE_THROWS_AS(null.ram(), std::out_of_range);
        REQUIRE_NOTHROW(rs.ram());
    }

    SECTION("logger") {
        REQUIRE_THROWS_AS(defaulted.logger(), std::runtime_error);
        REQUIRE(rs.logger() == log);
        REQUIRE(null.logger() == log);
    }

    SECTION("null") {
        REQUIRE(defaulted.null());
        REQUIRE_FALSE(rs.null());
    }

    SECTION("empty") {
        REQUIRE(defaulted.empty());
        REQUIRE_FALSE(rs.empty());
    }

    SECTION("swap") {
        ResourceSet defaulted_copy(defaulted);
        ResourceSet rs_copy(rs);
        defaulted_copy.swap(rs_copy);
        REQUIRE(rs == defaulted_copy);
        REQUIRE(defaulted == rs_copy);
    }

    SECTION("operator==/operator!=") {
        // Defaulted vs. defaulted
        ResourceSet other_defaulted;
        REQUIRE(defaulted == other_defaulted);
        REQUIRE_FALSE(defaulted != other_defaulted);

        // Defaulted vs. non-defaulted
        REQUIRE_FALSE(defaulted == rs);
        REQUIRE(defaulted != rs);

        // non-defaulted vs. non-defaulted (same state)
        ResourceSet other_rs(
          std::make_unique<pimpl_type>(rs.mpi_rank(), comm, log));
        REQUIRE(rs == other_rs);
        REQUIRE_FALSE(rs != other_rs);

        // Different ranks
        if(comm.size() > 1) {
            // Will grab rank 0 for all ranks > 0 and rank 1 for rank 0
            const auto& other = world.at(comm.me() ? 0 : 1);
            REQUIRE_FALSE(rs == other);
            REQUIRE(rs != other);
        }
    }
}
