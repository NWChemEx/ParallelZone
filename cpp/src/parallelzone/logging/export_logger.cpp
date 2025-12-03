/*
 * Copyright 2023 NWChemEx-Project
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

#include "export_logging.hpp"
#include <parallelzone/logging/logger.hpp>
#include <pybind11/operators.h>

namespace parallelzone {

void export_logger(pybind11::module_& m) {
    using const_string_reference = Logger::const_string_reference;
    using severity               = Logger::severity;

    using log0 = Logger& (Logger::*)(const_string_reference);
    using log1 = Logger& (Logger::*)(severity, const_string_reference);

    pybind11::class_<Logger> logger(m, "Logger");
    logger.def(pybind11::init<>())
      .def("set_severity", &Logger::set_severity)
      .def("trace", &Logger::trace)
      .def("debug", &Logger::debug)
      .def("info", &Logger::info)
      .def("warn", &Logger::warn)
      .def("error", &Logger::error)
      .def("critical", &Logger::critical)
      .def("log", static_cast<log0>(&Logger::log))
      .def("log", static_cast<log1>(&Logger::log))
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);

    pybind11::enum_<severity>(logger, "severity")
      .value("trace", severity::trace)
      .value("debug", severity::debug)
      .value("info", severity::info)
      .value("warn", severity::warn)
      .value("error", severity::error)
      .value("critical", severity::critical);
}

} // namespace parallelzone
