#include <catch2/catch.hpp>
#include <parallelzone/runtime/detail_/resource_set_pimpl.hpp>

using namespace parallelzone::runtime::detail_;

TEST_CASE("ResourceSetPIMPL") {
    // N.B. At this level in the hierarchy there's no check for whether the
    //      comm actually has more than one rank so these tests should work
    //      regardless of many ranks are used for testing

    using comm_type = ResourceSetPIMPL::mpi_comm_type;
    comm_type comm(MPI_COMM_WORLD);

    ResourceSetPIMPL rank0(0, comm);
    ResourceSetPIMPL rank1(1, comm);

    SECTION("CTor") {
        REQUIRE(rank0.m_rank == 0);
        REQUIRE(rank1.m_rank == 1);

        using parallelzone::hardware::detail_::make_ram;
        REQUIRE(rank0.m_ram == make_ram(get_ram_size(), 0, rank0.m_my_mpi));
        REQUIRE(rank1.m_ram == make_ram(get_ram_size(), 1, rank1.m_my_mpi));

        REQUIRE(rank0.m_my_mpi == comm);
        REQUIRE(rank1.m_my_mpi == comm);

        using logger_pointer = ResourceSetPIMPL::logger_pointer;
        REQUIRE(rank0.m_progress_logger_pointer == logger_pointer{});
        REQUIRE(rank1.m_progress_logger_pointer == logger_pointer{});

        REQUIRE(rank0.m_debug_logger_pointer == logger_pointer{});
        REQUIRE(rank1.m_debug_logger_pointer == logger_pointer{});
    }

    SECTION("clone") {
        REQUIRE(*rank0.clone() == rank0);
        REQUIRE(*rank1.clone() == rank1);
    }

    SECTION("progress_logger") {
        REQUIRE_THROWS_AS(rank0.progress_logger(), std::runtime_error);
        REQUIRE_THROWS_AS(rank1.progress_logger(), std::runtime_error);
    }

    SECTION("debug_logger") {
        REQUIRE_THROWS_AS(rank0.debug_logger(), std::runtime_error);
        REQUIRE_THROWS_AS(rank1.debug_logger(), std::runtime_error);
    }

    SECTION("operator==") {
        // Same
        REQUIRE(rank0 == ResourceSetPIMPL(0, comm));
        REQUIRE(rank1 == ResourceSetPIMPL(1, comm));

        // Different ranks
        REQUIRE_FALSE(rank0 == rank1);

        // Different MPI communicators
        REQUIRE_FALSE(rank0 == ResourceSetPIMPL(0, comm_type{}));
        REQUIRE_FALSE(rank1 == ResourceSetPIMPL(1, comm_type{}));

        // TODO: Compare loggers
    }
}

TEST_CASE("get_ram_size") { REQUIRE(get_ram_size() == 10); }
