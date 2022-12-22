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

#include "logger_pimpl.hpp"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <sstream>

namespace parallelzone::detail_ {

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

class SpdlogPIMPL : public LoggerPIMPL {
public:
    /// Type of a view of a Spdlog logger object
    using spdlog_ptr = std::shared_ptr<spdlog::logger>;

    /// Deleted to help avoid null state
    SpdlogPIMPL() = delete;

    /**
     * @brief Construct a new Spdlog P I M P L object
     *
     * @param[in] ptr The Spdlog logger we're wrapping. Assumed to be a non-
     *                null pointer.
     *
     * @throws std::runtime_error if @p ptr is a null pointer. Strong throw
     *                            guarantee.
     */
    explicit SpdlogPIMPL(spdlog_ptr ptr) : m_logger_(ptr) {
        if(!m_logger_) throw std::runtime_error("Expected non-null pointer.");
        m_logger_->set_level(spdlog::level::trace);
    }

private:
    /// Implements LoggerPIMPL::log by dispatching to spdlog::logger::log
    void log_(severity_type severity, const_string_reference msg) {
        m_logger_->log(map_severity_levels(severity), msg);
    }

    /// The actual spdlog logger
    std::shared_ptr<spdlog::logger> m_logger_;
};

inline auto make_stdout_color_mt(const std::string& name) {
    return SpdlogPIMPL(spdlog::stdout_color_mt(name));
}

inline auto make_file_mt(const std::string& file_name) {
    return SpdlogPIMPL(spdlog::basic_logger_mt(file_name, file_name));
}

} // namespace parallelzone::detail_