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
#include <parallelzone/runtime/resource_set.hpp>

namespace parallelzone::runtime {

void export_resource_set(python_module_type& m) {
    python_class_type<ResourceSet>(m, "ResourceSet")
      .def(py::init<>())
      .def("mpi_rank", &ResourceSet::mpi_rank)
      .def("is_mine", &ResourceSet::is_mine)
      .def("has_ram", &ResourceSet::has_ram)
      .def("ram", &ResourceSet::ram)
      .def("logger", &ResourceSet::logger)
      .def("null", &ResourceSet::null)
      .def("empty", &ResourceSet::empty)
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace parallelzone::runtime
