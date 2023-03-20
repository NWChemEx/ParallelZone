#pragma once
#include <pybind11/pybind11.h>

namespace parallelzone::runtime {

void export_runtime_view(pybind11::module_& m);
void export_resource_set(pybind11::module_& m);

inline void export_runtime(pybind11::module_& m) {
    auto mruntime = m.def_submodule("runtime");
    export_runtime_view(mruntime);
    export_resource_set(mruntime);
}

} // namespace parallelzone::runtime
