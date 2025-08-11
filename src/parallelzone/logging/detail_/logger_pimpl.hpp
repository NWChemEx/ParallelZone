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
#include <parallelzone/logging/logger.hpp>

namespace parallelzone::detail_ {

/**
 * @brief Defines the API for Logger implementations
 *
 * The LoggerPIMPL class decouples the `Logger` from the sink it logs to.
 * The details of the sink are implemented by classes which derive from
 * LoggerPIMPL.
 */
class LoggerPIMPL {
private:
    /// Type of class being implemented by *this
    using parent_type = Logger;

public:
    /// Ultimately a typedef of Logger::string_type;
    using string_type = parent_type::string_type;

    /// Ultimately a typedef of Logger::const_string_reference
    using const_string_reference = parent_type::const_string_reference;

    /// Ultimately a typedef of Logger::severity
    using severity_type = parent_type::severity;

    /// Ultimately a typedef of Logger::pimpl_ptr
    using pimpl_ptr = parent_type::pimpl_ptr;

    /// Standard no-throw default ctor
    LoggerPIMPL() = default;

    /// Standard polymorphic dtor
    virtual ~LoggerPIMPL() noexcept = default;

    /** @brief Makes a polymorphic deep copy of *this.
     *
     *  Calling this method will provide a deep copy of *this, returned by
     *  a pointer to its LoggerPIMPL base class. The exact details of what
     *  constitutes a deep copy depend on the derived class, but in general
     *  users should be aware that the sink in the deep copy is usually not
     *  copied on account of sinks being singletons. Deep copy in this sense
     *  refers to the meta-data associated with the logger (like the severity
     *  level it prints).
     *
     *  This method ultimately dispatches to clone_, which in turn is
     *  implemented by the derived class to do the copy.
     *
     *  @return A deep copy of *this, via a pointer to its base class.
     */
    pimpl_ptr clone() const { return clone_(); }

    /** @brief Used to set the severity threshold for logging.
     *
     *  All log messages have a severity level. To minimize noise users may
     *  choose to only display log messages with a certain severity level (or
     *  greater). This method sets the minimum severity log for display to
     *  @p severity. This means that log messages tagged with a severity of
     *  @p severity, or greater, will be displayed.
     *
     *  This method ultimately dispatches to set_severity_, which in turn
     *  is implemented by the derived class.
     *
     *  @param[in] severity The minimum severity level to display.
     *
     *  @throw ??? Throws if the derived class's implementation throws.
     */
    void set_severity(severity_type severity) { set_severity_(severity); }

    /** @brief Logs the provided message and associates with it the provided
     *         severity level.
     *
     *  This is method all logging methods of Logger make it to. The Logger
     *  class provides nicer APIs which users can use.
     *
     *  This method ultimately calls log_, which in turn is implemented by the
     *  derived class to do the logging. What actually happens when this method
     *  is called is highly dependent on what the derived class does. In
     *  general, users should expect the provided message to be logged unless
     *  the backend is setup to ignore messages at or below @p severity.
     *
     *  @param[in] severity How important is this message? See the Logger class
     *                      for options and meaning.
     *  @param[in] msg The message to log.
     *
     *  @throw ??? It is up to the backend whether this method throws or not
     *             and what guarantee it provides if it does throw.
     */
    void log(severity_type severity, const_string_reference msg) {
        log_(severity, msg);
    }

    /** @brief Used to polymorphically compare PIMPLs.
     *
     *  Exactly what value equality means depends on the backend. In general,
     *  two LoggerPIMPLs are polymorphically value equal if they have the
     *  same meta data associated with them and will write to the same sinks.
     *  Thus if two LoggerPIMPLs are value equal users can reasonably expect
     *  that writing to either PIMPL will have the same result.
     *
     *  @param[in] other The instance to compare to *this.
     *
     *  @return True if *this is polymorphically value equal to @p other and
     *          false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool are_equal(const LoggerPIMPL& other) const noexcept {
        return are_equal_(other) && other.are_equal_(*this);
    }

protected:
    // Protected to avoid slicing
    LoggerPIMPL(const LoggerPIMPL&)            = default;
    LoggerPIMPL& operator=(const LoggerPIMPL&) = default;
    LoggerPIMPL(LoggerPIMPL&&)                 = default;
    LoggerPIMPL& operator=(LoggerPIMPL&&)      = default;

    /** @brief Hook for polymorphic deep copy
     *
     *  The most derived class is responsible for making this method perform
     *  a deep copy of the entire class hierarchy and returning the copy via
     *  a pointer to the base of the hierarchy.
     *
     *  @return A deep polymorphic copy of *this.
     *
     *  @throw std::bad_alloc if there is a problem allocating the memory.
     *                        Derived classes should strive for strong throw
     *                        guarantee.
     */
    virtual pimpl_ptr clone_() const = 0;

    /** @brief Hook for setting the threshold severity for logging.
     *
     *  This method should be implemented by the derived class so that all
     *  messages tagged with a severity of at least @p severity get logged.
     *
     *  @param[in] severity The minimum severity for logging, going forward.
     *
     *  @throw ??? Derived classes may throw, although they should try their
     *             best not to.
     */
    virtual void set_severity_(severity_type severity) = 0;

    /** @brief Hook for actually logging.
     *
     *  The derived class should override this function so that it prints the
     *  provided message and associates the provided severity level with it.
     *
     *  @param[in] severity How important is this message? See Logger class
     *                      for options and details.
     *  @param[in] msg The message to log.
     *
     *  @throw ??? Derived classes may throw if there is a problem writing the
     *             message.
     */
    virtual void log_(severity_type severity, const_string_reference msg) = 0;

    /** @brief Hook for polymorphic value comparison
     *
     *  The most derived class is responsible for attempting to downcast
     *  @p other to the type of the most derived class. If the downcast fails
     *  return false, otherwise the derived class should polymorphically
     *  compare the state of *this with @p other.
     *
     *  @note LoggerPIMPL::are_equal ensures are_equal_ is called symmetrically.
     *        Derived classes can rely on this behavior in their implementation.
     *
     *  @return True if *this is polymorphically equal to @p other and false
     *          otherwise.
     *
     *  @throw None no throw guarantee.
     */
    virtual bool are_equal_(const LoggerPIMPL& other) const noexcept = 0;
};

} // namespace parallelzone::detail_
