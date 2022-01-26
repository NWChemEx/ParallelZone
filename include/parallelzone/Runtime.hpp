#pragma once

#include <madness/world/MADworld.h>
#include <madness/world/safempi.h>

#include <sstream>
#include <any>

namespace parallelzone {

class Runtime {

public:
  enum class Circumstances { NormalShutdown, Abort };

  /// Gets the type-erased MPI Communicator
  std::any mpi_comm();

  inline int getNumPartitions() { return num_partitions; }

  inline void setNumPartitions(int num) { num_partitions = num; }

/// @return true, if Runtime (and, necessarily, MADWorld runtime) is in an
/// initialized state
  inline bool isInitialized() { return initialized; }

private:
    Runtime();
    Runtime(int& argc, char**& argv);
    Runtime(int& argc, char**& argv, const MPI_Comm& comm);
    Runtime(int& argc, char**& argv, const SafeMPI::Intracomm& comm);

    ~Runtime();

    Runtime(const Runtime&) = delete;
    Runtime& operator=(const Runtime&) = delete;
    Runtime(Runtime&&)                 = delete;
    Runtime& operator=(Runtime&&) = delete;

    bool initialized;
    int num_partitions;
    madness::World& madness_world;
};

} // End namespace parallelzone
