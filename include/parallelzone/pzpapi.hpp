#pragma once

#include <papi.h>

namespace parallelzone {

  const int MAX_NUM_EVENTS = 32;

  /**
   * @brief Class Papi
   * @details Supports as a convinence wrapper around PAPI
   */
  class Papi {
  private:
    int m_eventset_{PAPI_NULL};
    long long m_values_[MAX_NUM_EVENTS] = {0};

    PAPI_event_info_t m_event_info_;

    // (non-owning) papi CPU device info
    PAPI_cpu_info_t* m_papi_cpu_info_{nullptr};

    // (non-owning) papi GPU device info
    // Limitation: currently supports only Nvidia, AMD
    PAPI_gpu_info_u* m_papi_gpu_info_{nullptr};

  public:
    /**
     * @brief Default constructor
     * @details Initializes PAPI and sets up Event sets
     */
    Papi();

    /**
     * @brief destructor
     * @details Destructs the PAPI appropriately
     */
    ~Papi();

    /**
     * @details Start PAPI event measurement
     */
    void start_papi_measurement();

    /**
     * @details Stop PAPI event measurement
     */
    void stop_papi_measurement();

    /**
     * @details resets PAPI event set measurement
     */    
    void reset_papi_measurement() noexcept { for (int i=0; i<MAX_NUM_EVENTS; ++i) m_values_[i] = 0; }

    Papi(const Papi&) = delete;
    Papi& operator=(const Papi&) = delete;
    Papi(Papi&&)                 = delete;
    Papi& operator=(Papi&&) = delete;
  };

} // namespace parallelzone
