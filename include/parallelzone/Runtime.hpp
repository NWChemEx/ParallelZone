#pragma once

#include <madness/world/MADworld.h>
#include <madness/world/safempi.h>

#include <any>
#include <sstream>

namespace parallelzone {

class Runtime {
public:
    /// @return MPI Communicator, associated with MADWorld
    MPI_Comm& mpi_comm() { return madness_world.mpi.comm().Get_mpi_comm(); }

    /// @return int, number of partitions; Defaults to 1
    constexpr int get_num_partitions() const noexcept { return num_partitions; }

    /// @return true, if Runtime (and, necessarily, MADWorld runtime) is in an
    /// initialized state
    constexpr bool is_initialized() const noexcept { return initialized; }

private:
    Runtime();
    Runtime(int argc, char** argv);
    Runtime(const MPI_Comm& comm);
    Runtime(const SafeMPI::Intracomm& comm);

    ~Runtime();

    Runtime(const Runtime&)            = delete;
    Runtime& operator=(const Runtime&) = delete;
    Runtime(Runtime&&)                 = delete;
    Runtime& operator=(Runtime&&)      = delete;

    bool initialized;
    int num_partitions;
    madness::World& madness_world;
};

} // End namespace parallelzone
