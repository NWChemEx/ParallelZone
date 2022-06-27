#pragma once

#include <memory>
#include <papi.h>

namespace parallelzone::detail_ {

  template <size_t EventSize>
  class PapiPIMPL {
  public:
    PapiPIMPL();

    /**
     * @details Start event measurement with PAPI library counters
     */
    void start_papi_measurement();

    /**
     * @details Stop event measurement with PAPI library counters
     */
    void stop_papi_measurement();

    /**
     * @details reset event measurement PAPI library counters
     */
    void reset_papi_measurement();
    
  private:
    std::array<long long, EventSize> m_values_{0};

    int m_eventset_;

    PAPI_event_info_t m_event_info_;

    // (non-owning) papi CPU device info
    PAPI_cpu_info_t* m_papi_cpu_info_{nullptr};

    // (non-owning) papi GPU device info
    PAPI_gpu_info_u* m_papi_gpu_info_{nullptr};
};

} // namespace parallelzone::detail_
