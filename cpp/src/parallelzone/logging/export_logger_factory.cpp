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

#include "export_logging.hpp"
#include <parallelzone/logging/logger_factory.hpp>

namespace parallelzone {

void export_logger_factory(pybind11::module_& m) {
    pybind11::class_<LoggerFactory>(m, "LoggerFactory")
      .def(pybind11::init<>())
      .def("default_global_logger", &LoggerFactory::default_global_logger);
}

} // namespace parallelzone
