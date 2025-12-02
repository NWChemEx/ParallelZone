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

#include "../export_hardware.hpp"
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
