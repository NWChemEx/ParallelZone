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
    /// Ultimately a typedef of Logger::pimpl_ptr
    using pimpl_ptr = parent_type::pimpl_ptr;

    /// Ultimately a typedef of Logger::ostream_reference
    using ostream_reference = parent_type::ostream_reference;

    LoggerPIMPL()                   = default;
    virtual ~LoggerPIMPL() noexcept = default;

    pimpl_ptr clone() const { return clone_(); }

    ostream_reference stream() { return stream_(); }

private:
    virtual pimpl_ptr clone_() const    = 0;
    virtual ostream_reference stream_() = 0;
};

} // namespace parallelzone::detail_
