/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "spdlog.hpp"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace parallelzone::detail_ {

/**
 * @brief Maps ParallelZone's severity levels to Spdlog's levels
 *
 * ParallelZone creates its own severity levels to isolate the user from the
 * backend's details. This function maps PZ's levels to Spdlog's.
 *
 * @param[in] s The ParallelZone severity level we are mapping from.
 * @return The Spdlog severity level corresponding to @p s.
 *
 * @throw std::logic_error if a new ParallelZone severity level is added and
 *                         not registered here. If all ParallelZone severity
 *                         levels are registered it is not possible to raise
 *                         this exception.
 */
inline auto map_severity_levels(Logger::severity s) {
    switch(s) {
        case(Logger::severity::trace): {
            return spdlog::level::trace;
        }
        case(Logger::severity::debug): {
            return spdlog::level::debug;
        }
        case(Logger::severity::info): {
            return spdlog::level::info;
        }
        case(Logger::severity::warn): {
            return spdlog::level::warn;
        }
        case(Logger::severity::error): {
            return spdlog::level::err;
        }
        case(Logger::severity::critical): {
            return spdlog::level::critical;
        }
    }

    throw std::logic_error("Did you forget to register a new severity?");
}

// -----------------------------------------------------------------------------
// -- SpdlogPIMPL Implementations
// -----------------------------------------------------------------------------

SpdlogPIMPL::SpdlogPIMPL(spdlog_type logger) : m_logger_(std::move(logger)) {
    m_logger_.set_level(spdlog::level::trace);
}

SpdlogPIMPL::pimpl_ptr SpdlogPIMPL::clone_() const {
    return std::unique_ptr<SpdlogPIMPL>(new SpdlogPIMPL(*this));
}

void SpdlogPIMPL::set_severity_(severity_type severity) {
    m_logger_.set_level(map_severity_levels(severity));
}

void SpdlogPIMPL::log_(severity_type severity, const_string_reference msg) {
    m_logger_.log(map_severity_levels(severity), msg);
}

bool SpdlogPIMPL::are_equal_(const LoggerPIMPL& other) const noexcept {
    auto p = dynamic_cast<const SpdlogPIMPL*>(&other);
    if(p == nullptr) return false;

    // As far as I can tell I can't get the levels to differ in two loggers
    // that have the same name and sinks. Maybe spdlog synchronizes them?

    // if(m_logger_.level() != p->m_logger_.level()) return false;

    return std::tie(m_logger_.name(), m_logger_.sinks()) ==
           std::tie(p->m_logger_.name(), p->m_logger_.sinks());
}

// -----------------------------------------------------------------------------
// -- Factory Functions
// -----------------------------------------------------------------------------

SpdlogPIMPL make_stdout_color_mt(const std::string& name) {
    auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    return SpdlogPIMPL(spdlog::logger(name, sink));
}

SpdlogPIMPL make_file_mt(const std::string& name,
                         const std::string& file_name) {
    auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(file_name);
    return SpdlogPIMPL(spdlog::logger(name, sink));
}

} // namespace parallelzone::detail_