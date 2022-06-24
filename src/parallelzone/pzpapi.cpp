#include "detail_/pzpapi_pimpl.hpp"
#include <parallelzone/pzpapi.hpp>

namespace parallelzone {

Papi::Papi() : m_pimpl_(std::make_unique<pimpl_type>()) {}

Papi::~Papi() { PAPI_shutdown(); }

void Papi::start_measurement() { m_pimpl_->start_papi_measurement(); }

void Papi::stop_measurement() { m_pimpl_->stop_papi_measurement(); }

void Papi::reset_measurement() noexcept { m_pimpl_->reset_papi_measurement(); }

} // namespace parallelzone
