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
#include <memory>
#include <ostream>
#include <string>

namespace parallelzone {
namespace detail_ {
/// Logger Implemenetation Class
class LoggerPIMPL;
} // namespace detail_

/** @brief A class to manage various loggers under a unified API
 *
 *  In our design, users determine what records/data to log by passing them to
 *  a logger. The logger then sends the record/data to a sink, which is
 *  responsible for actually logging the record/data. The Logger class
 *  implements the user-facing API for logging records/data.
 */
class Logger {
public:
    enum class severity { trace, debug, info, warn, error, critical };

    /// Type of the object used to implement a Logger instance
    using pimpl_type = detail_::LoggerPIMPL;

    /// Type of a unique_ptr to an object of type pimpl_type
    using pimpl_ptr = std::unique_ptr<pimpl_type>;

    /// Type of a string-based message
    using string_type = std::string;

    /// Type of a read-only reference to a string-based message
    using const_string_reference = const string_type&;

    /// Type of a raw output stream
    using ostream_type = std::ostream;

    /// Type of a read/write reference to an object of type ostream_type
    using ostream_reference = ostream_type&;

    /// Construct a Logger instance without implementation
    Logger();

    /**
     * @brief Construct a Logger instance from an implementation
     * pointer.
     *
     * @param p Implementation pointer
     * */
    Logger(pimpl_ptr&& p);

    /// Destruct Logger instance
    ~Logger() noexcept;

    /**
     * @brief Construct Logger instance via copy.
     *
     * @param[in] other Logger instance from which to construct
     *                  this instance.
     */
    Logger(const Logger& other);

    /**
     * @brief Construct Logger instance via move.
     *
     * @param[in] other Logger instance from which to construct
     *                  this instance.
     */
    Logger(Logger&& other) noexcept;

    /**
     * @brief Copy contents of Logger instance to this instance.
     *
     * @param[in] other Logger instance from which to copy
     * @returns   Reference to this instance
     */
    Logger& operator=(const Logger& other);

    /**
     * @brief Move contents of Logger instance to this instance.
     *
     * @param[in] other Logger instance from which to move
     * @returns   Reference to this instance
     */
    Logger& operator=(Logger&& other) noexcept;

    /**
     * @brief Print a message to log
     * @param[in] msg Message to print to log
     * @returns Reference to this logger instance
     */
    Logger& print(const_string_reference msg);

private:
    /// Pointer to implementatin details
    pimpl_ptr m_pimpl_;
};

} // namespace parallelzone
