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

#include "runtime.hpp"
#include <parallelzone/runtime/runtime_view.hpp>

namespace parallelzone::runtime {

void export_runtime_view(python_module_type& m) {
    python_class_type<RuntimeView>(m, "RuntimeView")
      .def(py::init<>())
      .def("size", &RuntimeView::size)
      .def("null", &RuntimeView::null)
      .def("did_i_start_mpi", &RuntimeView::did_i_start_mpi)
      .def("at", &RuntimeView::at)
      .def("has_me", &RuntimeView::has_me)
      .def("my_resource_set", &RuntimeView::my_resource_set)
      .def("count", &RuntimeView::count)
      .def("logger", &RuntimeView::logger,
           py::return_value_policy::reference_internal)
      .def("stack_callback", &RuntimeView::stack_callback)
      .def(py::self == py::self)
      .def(py::self != py::self);
}
} // namespace parallelzone::runtime
