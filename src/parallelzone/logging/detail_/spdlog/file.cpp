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
#include "file.hpp"
#include <spdlog/sinks/basic_file_sink.h>

namespace parallelzone::detail_ {

using sink_type = spdlog::sinks::basic_file_sink_mt;

FileSpdlog::FileSpdlog(const_string_reference id,
                       const_string_reference file_name) :
  SpdlogPIMPL(spdlog::logger(id, std::make_shared<sink_type>(file_name))),
  m_file_name_(file_name) {}

FileSpdlog::pimpl_ptr FileSpdlog::clone_() const {
    return std::unique_ptr<FileSpdlog>(new FileSpdlog(*this));
}

bool FileSpdlog::are_equal_(const LoggerPIMPL& other) const noexcept {
    auto p = dynamic_cast<const FileSpdlog*>(&other);
    if(p == nullptr) return false;

    if(m_file_name_ != p->m_file_name_) return false;
    return SpdlogPIMPL::are_equal_(other);
}

} // namespace parallelzone::detail_