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

#include "hardware/export_hardware.hpp"
#include "logging/export_logging.hpp"
#include "runtime/export_runtime.hpp"
#include <pybind11/pybind11.h>

namespace parallelzone {

PYBIND11_MODULE(parallelzone, m) {
    m.doc() = "PyPZ : Python bindings for ParallelZone";

    export_logging(m);
    hardware::export_hardware(m);
    runtime::export_runtime(m);
}

} // namespace parallelzone
