#include "pzpapi_pimpl.hpp"
#include <cstring>
#include <stdexcept>

namespace parallelzone::detail_ {

  template <size_t EventSize>
  PapiPIMPL<EventSize>::PapiPIMPL() : m_eventset_(PAPI_NULL) {
    int retVal;
    retVal = PAPI_library_init(static_cast<int>(PAPI_VER_CURRENT));
    if(retVal != PAPI_VER_CURRENT) {
        throw std::runtime_error("PAPI_library_init() failed!");
    }

    const PAPI_hw_info_t* papi_hwinfo   = PAPI_get_hardware_info();
    PAPI_dev_type_info_t* dev_type_info = papi_hwinfo->dev_type_arr;

    for(int dev_type_id = 0; dev_type_id < PAPI_DEV_TYPE_ID__MAX_NUM;
        dev_type_id++) {
        // CPU
        if(dev_type_info[dev_type_id].id == PAPI_DEV_TYPE_ID__CPU &&
           dev_type_info[dev_type_id].num_devices > 0) {
            m_papi_cpu_info_ = reinterpret_cast<PAPI_cpu_info_t*>(
              dev_type_info[dev_type_id].dev_info_arr);
        }
        // GPU (Nvidia, AMD)
        if((dev_type_info[dev_type_id].id == PAPI_DEV_TYPE_ID__NVIDIA_GPU ||
            dev_type_info[dev_type_id].id == PAPI_DEV_TYPE_ID__AMD_GPU) &&
           dev_type_info[dev_type_id].num_devices > 0) {
            m_papi_gpu_info_ = reinterpret_cast<PAPI_gpu_info_u*>(
              dev_type_info[dev_type_id].dev_info_arr);
        }
    } // for dev_type_id

    PAPI_create_eventset(&m_eventset_);

    int cid = -1;
    // Find cuda,rocm,intel component index (get number of components)
    for(int i = 0; i < PAPI_num_components() && cid < 0; i++) { // while not found,
      // get the component info
      PAPI_component_info_t* aComponent =
	(PAPI_component_info_t*)PAPI_get_component_info(i);
      if(aComponent == nullptr) {
	throw std::runtime_error(
	  "PAPI_get_component_info failed, returned NULL!");
      }

      // If we found our match, record it.
      if((strcmp("cuda", aComponent->name) == 0) ||
	 (strcmp("rocm", aComponent->name) == 0) ||
	 (strcmp("intel_gpu", aComponent->name) == 0)) {
	cid = i;
      }
    } // end search components.

    if(cid < 0) { // if no PCP component found,
        throw std::runtime_error(
          "Failed to find GPU component among reported components!");
    }

    PAPI_assign_eventset_component(m_eventset_, cid);
}

  template <size_t EventSize>  
  void PapiPIMPL<EventSize>::start_papi_measurement() {
    int retVal = PAPI_start(m_eventset_);
    if(retVal != PAPI_OK) {
        throw std::runtime_error("Papi::start_papi_measurement() failed!");
    }
}

  template <size_t EventSize>
  void PapiPIMPL<EventSize>::stop_papi_measurement() {
    long long values[EventSize];
    int retVal = PAPI_stop(m_eventset_, values);
    if(retVal != PAPI_OK) {
        throw std::runtime_error("Papi::stop_papi_measurement() failed!");
    }

    for(int i = 0; i < EventSize; ++i) { m_values_[i] += values[i]; }
}

  template <size_t EventSize>  
  void PapiPIMPL<EventSize>::reset_papi_measurement() {
    m_values_.fill(0);
}

} // namespace parallelzone::detail_
