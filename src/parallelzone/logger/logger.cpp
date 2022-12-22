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

Logger::Logger() : Logger(nullptr) {}
Logger::Logger(pimpl_ptr&& p) : m_pimpl_(std::move(p)) {}
Logger::~Logger() noexcept = default;

Logger::Logger(Logger&&) noexcept            = default;
Logger& Logger::operator=(Logger&&) noexcept = default;

#define ASSERT_PIMPL(p) \
    if(!p) throw std::runtime_error("LoggerPIMPL not set");

Logger::Logger(const Logger& other) :
  m_pimpl_(other.m_pimpl_ ? other.m_pimpl_->clone() : nullptr) {}

Logger& Logger::operator=(const Logger& other) {
    Logger(other).m_pimpl_.swap(m_pimpl_);
    return *this;
}

Logger& Logger::print(const_string_reference msg) {
    if(m_pimpl_) m_pimpl_->log(severity::info, msg);
    return *this;
}

#undef ASSERT_PIMPL

} // namespace parallelzone
