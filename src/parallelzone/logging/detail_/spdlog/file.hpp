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

#include "spdlog.hpp"

namespace parallelzone::detail_ {

/** @brief Logger which writes to file.
 *
 *  This class specializes SpdlogPIMPL for when the sink is a file. Under the
 *  hood we use Spdlog to do the actual logging.
 */
class FileSpdlog : public SpdlogPIMPL {
public:
    /** @brief Creates a LoggerPIMPL whose sink is the specified file.

     *
     *  The object resulting from this function is setup so that logged records
     *  are written directly to the provided file. If the file already exists
     *  the resulting logger will append to it.
     *
     *  @param[in] id An ID for the logger.
     *  @param[in] file_name The path where the file will exist. If @p file_name
     *                       is relative it will be created relative to where
     *                       the program is currently running.
     *
     *  @return SpdlogPIMPL wrapping an spdlog::logger setup according to this
     *          function's description.
     */
    FileSpdlog(const_string_reference id, const_string_reference file_name);

protected:
    FileSpdlog(const FileSpdlog&) = default;
    FileSpdlog& operator=(const FileSpdlog&) = default;
    FileSpdlog(FileSpdlog&&)                 = default;
    FileSpdlog& operator=(FileSpdlog&&) = default;

    /// Implemented by calling the copy ctor on *this
    pimpl_ptr clone_() const override;

    /// If other is FileSpdlog object, compares file names then calls
    /// SpdlogPIMPL::are_equal_(other)
    bool are_equal_(const LoggerPIMPL& other) const noexcept override;

    /// Copy of the file name for comparisons
    string_type m_file_name_;
};

} // namespace parallelzone::detail_
