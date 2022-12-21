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

class SpdlogPIMPL : public LoggerPIMPL {
public:
    using spdlog_ptr = std::shared_ptr<spdlog::logger>;

    SpdlogPIMPL() = default;

    explicit SpdlogPIMPL(spdlog_ptr ptr) : m_logger_(ptr) {}

private:
    void trace_(const_string_reference msg) override {
        if(!m_logger_) return;
        m_logger_->trace(msg);
    }

    void debug_(const_string_reference msg) override {
        if(!m_logger_) return;
        m_logger_->debug(msg);
    }

    void info_(const_string_reference msg) override {
        if(!m_logger_) return;
        m_logger_->info(msg);
    }

    void warn_(const_string_reference msg) override {
        if(!m_logger_) return;
        m_logger_->warn(msg);
    }

    void error_(const_string_reference msg) override {
        if(!m_logger_) return;
        m_logger_->error(msg);
    }

    void critical_(const_string_reference msg) override {
        if(!m_logger_) return;
        m_logger_->critical(msg);
    }

    /// The actual spdlog logger
    std::shared_ptr<spdlog::logger> m_logger_;
};

inline auto make_stdout_color_mt() {
    return SpdlogPIMPL(spdlog::stdout_color_mt("stdout"));
}

inline auto make_file_mt(const std::string& file_name) {
    return SpdlogPIMPL(spdlog::basic_logger_mt(file_name, file_name));
}

inline auto make_sstream_mt(const std::string& name, std::stringstream& ss) {
    auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(ss);
    return SpdlogPIMPL(std::make_shared<spdlog::logger>(name, sink));
}

} // namespace parallelzone::detail_