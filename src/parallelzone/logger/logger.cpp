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

#include "detail_/logger_pimpl.hpp"
#include <fstream>
#include <iostream>

namespace parallelzone {

// -----------------------------------------------------------------------------
// -- Ctors, Dtor, Assignment
// -----------------------------------------------------------------------------

Logger::Logger() noexcept : Logger(nullptr) {}
Logger::Logger(pimpl_ptr&& p) noexcept : m_pimpl_(std::move(p)) {}
Logger::~Logger() noexcept = default;

Logger::Logger(Logger&&) noexcept = default;
Logger& Logger::operator=(Logger&&) noexcept = default;

Logger::Logger(const Logger& other) :
  m_pimpl_(other.m_pimpl_ ? other.m_pimpl_->clone() : nullptr) {}

Logger& Logger::operator=(const Logger& other) {
    Logger(other).swap(*this);
    return *this;
}

void Logger::set_severity(severity s) {
    if(has_pimpl_()) m_pimpl_->set_severity(s);
}

Logger& Logger::trace(const_string_reference msg) {
    return log(severity::trace, msg);
}

Logger& Logger::debug(const_string_reference msg) {
    return log(severity::debug, msg);
}

Logger& Logger::info(const_string_reference msg) {
    return log(severity::info, msg);
}

Logger& Logger::warn(const_string_reference msg) {
    return log(severity::warn, msg);
}

Logger& Logger::error(const_string_reference msg) {
    return log(severity::error, msg);
}

Logger& Logger::critical(const_string_reference msg) {
    return log(severity::critical, msg);
}

Logger& Logger::log(const_string_reference msg) { return info(msg); }

Logger& Logger::log(severity s, const_string_reference msg) {
    if(m_pimpl_) m_pimpl_->log(s, msg);
    return *this;
}

Logger& Logger::operator<<(const_string_reference msg) { return log(msg); }

// -----------------------------------------------------------------------------
// -- Utility
// -----------------------------------------------------------------------------

void Logger::swap(Logger& other) noexcept { m_pimpl_.swap(other.m_pimpl_); }

bool Logger::operator==(const Logger& rhs) const noexcept {
    if(has_pimpl_() != rhs.has_pimpl_()) return false;
    if(!has_pimpl_()) return true; // Both are null
    return m_pimpl_->are_equal(*rhs.m_pimpl_);
}

bool Logger::operator!=(const Logger& rhs) const noexcept {
    return !(*this == rhs);
}

// -----------------------------------------------------------------------------
// -- Private methods
// -----------------------------------------------------------------------------

bool Logger::has_pimpl_() const noexcept { return static_cast<bool>(m_pimpl_); }

} // namespace parallelzone
