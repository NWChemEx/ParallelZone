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

/** @brief Implements LoggerPIMPL so the sink is standard out.
 *
 *  This class is more or less a strong type so we can tell the Spdlog backend
 *  apart.
 */
class StdoutSpdlog : public SpdlogPIMPL {
public:
    /** @brief Creates a LoggerPIMPL whose sink is standard out
     *
     *  The object resulting from this function is setup so that logged records
     *  are directed to standard out, in color, and in a thread-safe manner.
     *  Spdlog is used to actually implement the logger.
     *
     *  @param[in] id An ID for the logger, used to distinguish loggers in the
     *                output.
     */
    explicit StdoutSpdlog(const_string_reference id);

protected:
    StdoutSpdlog(const StdoutSpdlog&)            = default;
    StdoutSpdlog& operator=(const StdoutSpdlog&) = default;
    StdoutSpdlog(StdoutSpdlog&&)                 = default;
    StdoutSpdlog& operator=(StdoutSpdlog&&)      = default;

    /// Implements clone() by calling copy ctor on *this
    pimpl_ptr clone_() const override;

    /// Defers to SpdLogPIMPL::are_equal_ if @p other is StdoutSpdlog object
    bool are_equal_(const LoggerPIMPL& other) const noexcept override;
};

} // namespace parallelzone::detail_
