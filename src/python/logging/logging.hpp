#pragma once
#include <pybind11/pybind11.h>

namespace parallelzone {

void export_logger_factory(pybind11::module_& m);
void export_logger(pybind11::module_& m);

inline void export_logging(pybind11::module_& m) {
    export_logger_factory(m);
    export_logger(m);
}

} // namespace parallelzone
