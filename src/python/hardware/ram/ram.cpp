#include "../hardware.hpp"
#include <parallelzone/hardware/ram/ram.hpp>
#include <pybind11/operators.h>

namespace parallelzone::hardware {

void export_ram(pybind11::module_& m) {
    using hardware::RAM;

    pybind11::class_<RAM>(m, "RAM")
      .def(pybind11::init<>())
      .def("total_space", &RAM::total_space)
      .def("empty", &RAM::empty)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace parallelzone::hardware
