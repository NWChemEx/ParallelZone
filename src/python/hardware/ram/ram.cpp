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

#include "../hardware.hpp"
#include <parallelzone/hardware/ram/ram.hpp>

namespace parallelzone::hardware {

void export_ram(python_module_type& m) {
    using hardware::RAM;

    python_class_type<RAM>(m, "RAM")
      .def(py::init<>())
      .def("total_space", &RAM::total_space)
      .def("empty", &RAM::empty)
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace parallelzone::hardware
