#include <parallelzone/runtime.hpp>
#include <catch2/catch.hpp>

#include <mpi.h>

TEST_CASE("Runtime Logger","[runtime][logger]") {
    auto rt = parallelzone::Runtime();

    int rank; MPI_Comm_rank( rt.mpi_comm(), &rank );


    SECTION("Progress Logger") {
        std::stringstream ss;
	if(!rank)
          rt.set_progress_logger(parallelzone::make_stream_logger(&ss));
        rt.progress_logger().stream() << "Hello World" << std::flush;
	if(rank) REQUIRE(ss.str().size() == 0);
	else     REQUIRE(ss.str() == "Hello World");
    }

    SECTION("Debug Logger") {
        std::stringstream ss;
        rt.set_debug_logger(parallelzone::make_stream_logger(&ss));
        rt.debug_logger().stream() << "Hello World " << rank << std::flush;

	std::string ref_msg = "Hello World " + std::to_string(rank);
	REQUIRE(ss.str() == ref_msg);
    }
}
