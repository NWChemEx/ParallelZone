#pragma once
#include <pybind11/pybind11.h>

namespace parallelzone::hardware {

void export_ram(pybind11::module_& m);

inline void export_hardware(pybind11::module_& m) {
    auto mhardware = m.def_submodule("hardware");
    export_ram(mhardware);
}

} // namespace parallelzone::hardware
