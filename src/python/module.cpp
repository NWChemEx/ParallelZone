#include "hardware/hardware.hpp"
#include "logging/logging.hpp"
#include "runtime/runtime.hpp"
#include <pybind11/pybind11.h>

namespace parallelzone {

PYBIND11_MODULE(parallelzone, m) {
    m.doc() = "PyPZ : Python bindings for ParallelZone";

    hardware::export_hardware(m);
    export_logging(m);
    runtime::export_runtime(m);
}

} // namespace parallelzone
