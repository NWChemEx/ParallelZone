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

class LoggerPIMPL {
private:
    /// Type of class being implemented by *this
    using parent_type = Logger;

public:
    /// Ultimately a typedef of Logger::const_string_reference
    using const_string_reference = std::string;

    /// Ultimately a typedef of Logger::pimpl_ptr
    using pimpl_ptr = parent_type::pimpl_ptr;

    LoggerPIMPL()                              = default;
    LoggerPIMPL(const LoggerPIMPL&)            = delete;
    LoggerPIMPL& operator=(const LoggerPIMPL&) = delete;
    virtual ~LoggerPIMPL() noexcept            = default;

    void trace(const_string_reference msg) { trace_(msg); }
    void debug(const_string_reference msg) { debug_(msg); }
    void info(const_string_reference msg) { info_(msg); }
    void warn(const_string_reference msg) { warn_(msg); }
    void error(const_string_reference msg) { error_(msg); }
    void critical(const_string_reference msg) { critical_(msg); }

protected:
    // Protected to avoid slicing
    LoggerPIMPL(LoggerPIMPL&&)            = default;
    LoggerPIMPL& operator=(LoggerPIMPL&&) = default;

private:
    virtual void trace_(const_string_reference msg) = 0;

    virtual void debug_(const_string_reference msg) = 0;

    virtual void info_(const_string_reference msg) = 0;

    virtual void warn_(const_string_reference msg) = 0;

    virtual void error_(const_string_reference msg) = 0;

    virtual void critical_(const_string_reference msg) = 0;
};

} // namespace parallelzone::detail_
