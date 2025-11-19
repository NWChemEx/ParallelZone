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

#include "hardware/hardware.hpp"
#include "logging/logging.hpp"
#include "runtime/runtime.hpp"
#include <pybind11/pybind11.h>

namespace parallelzone {

PYBIND11_MODULE(parallelzone, m) {
    m.doc() = "PyPZ : Python bindings for ParallelZone";

    hardware::export_hardware(m);
    export_logging(m);
    runtime::export_runtime(m);
}

} // namespace parallelzone
