#pragma once

#include <parallelzone/hardware.hpp>

namespace parallelzone {

class Worker {
public:
    /**
     * @brief Default constructor
     * @details Worker instance initializes MADness runtime, sets partitions
     * to 1.
     */
  Worker();

private:
  Hardware& hardware;
};

} // End namespace parallelzone
