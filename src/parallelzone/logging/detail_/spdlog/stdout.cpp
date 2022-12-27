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

#include "stdout.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace parallelzone::detail_ {

StdoutSpdlog::StdoutSpdlog(const_string_reference id) :
  SpdlogPIMPL(spdlog::logger(
    id, std::make_shared<spdlog::sinks::stdout_color_sink_mt>())) {}

StdoutSpdlog::pimpl_ptr StdoutSpdlog::clone_() const {
    return std::unique_ptr<StdoutSpdlog>(new StdoutSpdlog(*this));
}

bool StdoutSpdlog::are_equal_(const LoggerPIMPL& other) const noexcept {
    auto p = dynamic_cast<const StdoutSpdlog*>(&other);
    if(p == nullptr) return false;
    return SpdlogPIMPL::are_equal_(other);
}

} // namespace parallelzone::detail_