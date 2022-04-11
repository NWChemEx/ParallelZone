#pragma once

#include <papi.h>
#include <parallelzone/worker.hpp>
#include <parallelzone/hardware.hpp>
#include <functional>

namespace parallelzone {

class Resourceset {
public:
    /**
     * @brief Default constructor
     * @details Resourceset instance initializes MADness runtime, sets partitions
     * to 1.
     */
    Resourceset();

//     /// @return Worker containing current process
//     Worker& me() { return mad_world.mpi.comm().Get_mpi_comm(); }

//     /// @return MADWorld, ref handle to madness::World
//     madness::World& madness_world() { return mad_world; }

//     std::string name() {}
  
//     /// @return int, number of partitions; Defaults to 1
//     constexpr int get_num_partitions() const noexcept { return num_partitions; }

//     /// @return true, if Resourceset (and, necessarily, MADWorld runtime) is in an
//     /// initialized state
//     constexpr bool is_initialized() const noexcept { return initialized; }

//     /**
//      * @brief destructor
//      * @details Destructs the MADness runtime appropriately, if Resourceset is
//      * initialized
//      */
//     ~Resourceset();

//     Resourceset(const Resourceset&) = delete;
//     Resourceset& operator=(const Resourceset&) = delete;
//     Resourceset(Resourceset&&)                 = delete;
//     Resourceset& operator=(Resourceset&&) = delete;

private:
  const PAPI_hw_info_t* papi_info;
  //const PAPI_hw_info_t& papi_info;  
  //std::reference_wrapper<const PAPI_hw_info_t&> papi_info;
//  Worker& worker;
  Hardware<CPU>& cpu_hardware;
  Hardware<GPU>& gpu_hardware;  
};

} // End namespace parallelzone
