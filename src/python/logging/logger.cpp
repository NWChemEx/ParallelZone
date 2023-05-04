#include "logging.hpp"
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
