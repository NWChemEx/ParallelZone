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
