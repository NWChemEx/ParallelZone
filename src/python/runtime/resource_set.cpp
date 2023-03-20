#include "runtime.hpp"
#include <parallelzone/runtime/resource_set.hpp>
#include <pybind11/operators.h>

namespace parallelzone::runtime {

void export_resource_set(pybind11::module_& m) {
    pybind11::class_<ResourceSet>(m, "ResourceSet")
      .def(pybind11::init<>())
      .def("mpi_rank", &ResourceSet::mpi_rank)
      .def("is_mine", &ResourceSet::is_mine)
      .def("has_ram", &ResourceSet::has_ram)
      .def("ram", &ResourceSet::ram)
      .def("logger", &ResourceSet::logger)
      .def("null", &ResourceSet::null)
      .def("empty", &ResourceSet::empty)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}
} // namespace parallelzone::runtime
