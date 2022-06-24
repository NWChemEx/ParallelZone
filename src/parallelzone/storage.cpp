#include "detail_/storage_pimpl.hpp"

namespace parallelzone {

  Storage::Storage()
    : m_pimpl_(std::make_unique<pimpl_type>()) {}

  Storage::~Storage() {
    PAPI_shutdown();
  }

  void Storage::start_measurement() {
    m_pimpl_->start_papi_measurement();
  }

  void Storage::stop_measurement() {
    m_pimpl_->stop_papi_measurement(); 
  }

  void Storage::reset_measurement() noexcept {
    m_pimpl_->reset_papi_measurement(); 
  }
  
} // namespace parallelzone
