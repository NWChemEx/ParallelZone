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

#pragma once

#include <parallelzone/logger/logger.hpp>

namespace parallelzone::detail_ {

/**
 * @brief Defines the API for LoggerView implementations
 *
 * The LoggerPIMPL class decouples the ``LoggerView`` from
 */
class LoggerPIMPL {
private:
    /// Type of class being implemented by *this
    using parent_type = Logger;

public:
    /// Ultimately a typedef of Logger::const_string_reference
    using const_string_reference = std::string;

    /// Ultimately a typedef of Logger::severity
    using severity_type = parent_type::severity;

    LoggerPIMPL()                              = default;
    LoggerPIMPL(const LoggerPIMPL&)            = delete;
    LoggerPIMPL& operator=(const LoggerPIMPL&) = delete;
    LoggerPIMPL(LoggerPIMPL&&)                 = delete;
    LoggerPIMPL& operator=(LoggerPIMPL&&)      = delete;
    virtual ~LoggerPIMPL() noexcept            = default;

    void log(severity_type severity, const_string_reference msg) {
        log_(severity, msg);
    }

protected:
    // Protected to avoid slicing

private:
    virtual void log_(severity_type severity, const_string_reference msg) = 0;
};

} // namespace parallelzone::detail_
