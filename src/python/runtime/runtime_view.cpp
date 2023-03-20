#include "runtime.hpp"
#include <parallelzone/runtime/runtime_view.hpp>
#include <pybind11/operators.h>

namespace parallelzone::runtime {

void export_runtime_view(pybind11::module_& m) {
    pybind11::class_<RuntimeView>(m, "RuntimeView")
      .def(pybind11::init<>())
      .def("size", &RuntimeView::size)
      .def("null", &RuntimeView::null)
      .def("did_i_start_madness", &RuntimeView::did_i_start_madness)
      .def("at", &RuntimeView::at)
      .def("has_me", &RuntimeView::has_me)
      .def("my_resource_set", &RuntimeView::my_resource_set)
      .def("count", &RuntimeView::count)
      .def("logger", &RuntimeView::logger)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}
} // namespace parallelzone::runtime
