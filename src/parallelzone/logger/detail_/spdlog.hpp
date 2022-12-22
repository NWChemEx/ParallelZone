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
#include <spdlog/spdlog.h>

namespace parallelzone::detail_ {

/** @brief LoggerPIMPL object which wraps a spdlog::logger
 *
 *  This class implements ParallelZone's logger by punting to spdlog. Internally
 *  it holds an spdlog::logger. Users of this PIMPL should set an spdlog::logger
 *  up to their liking and then make an SpdlogPIMPL. Free functions which wrap
 *  common scenarios: stdout and files are provided.
 */
class SpdlogPIMPL : public LoggerPIMPL {
public:
    /// Type of a view of a Spdlog logger object
    using spdlog_type = spdlog::logger;

    /// Deleted to help avoid null state
    SpdlogPIMPL() = delete;

    /** @brief Construct a new SpdlogPIMPL object which wraps @p ptr
     *
     * SpdlogPIMPL objects work by wrapping Spdlog::logger objects. This ctor
     * takes an already created Spdlog::logger object and builds *this around
     * it.
     *
     * @param[in] ptr The Spdlog logger we're wrapping. Assumed to be a non-
     *                null pointer.
     *
     * @throws std::runtime_error if @p ptr is a null pointer. Strong throw
     *                            guarantee.
     */
    explicit SpdlogPIMPL(spdlog_type logger);

protected:
    SpdlogPIMPL(const SpdlogPIMPL&)            = default;
    SpdlogPIMPL& operator=(const SpdlogPIMPL&) = default;
    SpdlogPIMPL(SpdlogPIMPL&&)                 = default;
    SpdlogPIMPL& operator=(SpdlogPIMPL&&)      = default;

private:
    /// Calls the copy ctor on *this
    pimpl_ptr clone_() const override;

    /// Implements set_severity_ by setting the level on m_logger_
    void set_severity_(severity_type severity) override;

    /// Implements LoggerPIMPL::log by dispatching to spdlog::logger::log
    void log_(severity_type severity, const_string_reference msg) override;

    /** @brief Implements LoggerPIMPL::are_equal by downcasting @p other and
     *         comparing m_logger_ to other.m_logger_
     *
     *  This method compares the name and sinks of the wrapped spdlog object.
     *  It does not compare the levels or pattern (the former seems to
     *  auto-synch and the latter is not exposed).
     *
     *  @param[in] other The object to compare to *this.
     *
     *  @return True if @p other is a Spdlog object and  @p other and *this
     *          have the same state. False otherwise.
     */
    bool are_equal_(const LoggerPIMPL& other) const noexcept override;

    /// The actual spdlog logger, shared_ptr b/c it functions like a singleton
    spdlog_type m_logger_;
};

// -----------------------------------------------------------------------------
// -- Factory functions
// -----------------------------------------------------------------------------

/** @brief Creates a SpdlogPIMPL whose sink is standard out
 *
 *  @relates SpdlogPIMPL
 *
 *  The object resulting from this function is setup so that logged records are
 *  directed to standard out, in color, and in a thread-safe manner.
 *
 *  @param[in] name An ID for the logger.
 *
 *  @return SpdlogPIMPL wrapping an spdlog::logger setup according to this
 *          function's description.
 */
SpdlogPIMPL make_stdout_color_mt(const std::string& name);

/** @brief Creates a SpdlogPIMPL whose sink is the specified file.
 *
 *  @relates SpdlogPIMPL
 *
 *  The object resulting from this function is setup so that logged records are
 *  written directly to the provided file. If the file already exists the
 *  resulting logger will append to it.
 *
 *  @param[in] name An ID for the logger.
 *
 *  @return SpdlogPIMPL wrapping an spdlog::logger setup according to this
 *          function's description.
 */
SpdlogPIMPL make_file_mt(const std::string& name, const std::string& file_name);

} // namespace parallelzone::detail_