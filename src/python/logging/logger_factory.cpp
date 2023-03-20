#include "logging.hpp"
#include <parallelzone/logging/logger_factory.hpp>

namespace parallelzone {

void export_logger_factory(pybind11::module_& m) {
    pybind11::class_<LoggerFactory>(m, "LoggerFactory")
      .def(pybind11::init<>())
      .def("default_global_logger", &LoggerFactory::default_global_logger);
}

} // namespace parallelzone
