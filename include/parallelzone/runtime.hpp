#pragma once

#include <madness/world/MADworld.h>
#include <madness/world/safempi.h>
#include <madness/world/world.h>
#include <parallelzone/resourceset.hpp>
#include <vector>

namespace parallelzone {

class Runtime {
public:
    /**
     * @brief Default constructor
     * @details If madness isn't initialized, initializes madness and sets
     * partitions to 1. Otherwise, references the madness default world.
     */
    Runtime();

    /**
     * @brief constructor
     * @details Runtime instance initializes MADness runtime, sets partitions
     * to 1.
     */
    Runtime(int argc, char** argv);

    /**
     * @brief constructor
     * @details Starts MADness using a MPI Communicator. Runtime instance
     * doesn't explicitly start MADness
     */
    Runtime(const MPI_Comm& comm);

    /**
     * @brief constructor
     * @details Starts MADness using a SafeMPI Communicator. Runtime instance
     * doesn't explicitly start MADness
     */
    Runtime(const SafeMPI::Intracomm& comm);

    /**
     * @brief constructor
     * @details Starts MADness using a SafeMPI Communicator. Runtime instance
     * doesn't explicitly start MADness
     */
    Runtime(int argc, char** argv, const SafeMPI::Intracomm& comm);

    /// @return MPI Communicator, associated with MADWorld
    MPI_Comm& mpi_comm() { return m_world_.mpi.comm().Get_mpi_comm(); }

    /// @return MADWorld, ref handle to madness::World
    madness::World& madness_world() { return m_world_; }

    /// @return int, number of partitions; Defaults to 1
    constexpr int num_partitions() const noexcept { return m_num_partitions_; }

    /// @return true if this instance initialized madness.
    constexpr bool started_madness() const noexcept { return m_init_madness_; }

    /// @return size_t, number of resourceset instances in this Runtime
    std::size_t size() const noexcept { return resource_sets.size(); }

    /// @return resourceset, returns a reference to the resourceset at position
    /// n among the list of several resourceset instances.
    Resourceset& operator[](std::size_t n) { return resource_sets[n]; };

    /// @return resourceset, returns a const reference to the resourceset at
    /// position n among the list of several resourceset instances.
    const Resourceset& operator[](std::size_t n) const {
        return resource_sets[n];
    };

    /// @return void, partitions the given runtime instance to several smaller
    /// resourceset instances. Default: partitions to 1 resourceset instance
    void partition();

    /**
     * @brief destructor
     * @details Destructs the MADness runtime appropriately, if Runtime
     * initialized MADness.
     */
    ~Runtime();

    Runtime(const Runtime&) = delete;
    Runtime& operator=(const Runtime&) = delete;
    Runtime(Runtime&&)                 = delete;
    Runtime& operator=(Runtime&&) = delete;

private:
    /// Tracks if this instance initialized madness
    bool m_init_madness_;

    /// The number of partitions
    int m_num_partitions_;

    /// Reference to the madness world this instance wraps
    madness::World& m_world_;
};

} // End namespace parallelzone
