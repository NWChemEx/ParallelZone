# Copyright 2023 NWChemEx-Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#[[[ @module NWChemEx pybind11 helpers
#
# This module:
#    1. Wraps the process of finding pybind11 in the appropriate configuration.
#    2. Defines a function ``nwx_pybind11_module`` to facilitate making a
#       Python module from an NWChemEx-like library
#    3. Defines a function ``nwx_python_tests`` to facilitate registering
#       Python-based tests with CTest
#]]


include_guard()

cmaize_find_or_build_dependency(
    pybind11
    URL github.com/pybind/pybind11
    BUILD_TARGET pybind11::pybind11
    FIND_TARGET pybind11::embed
    CMAKE_ARGS PYBIND11_INSTALL=ON
               PYBIND11_FINDPYTHON=ON
)

#[[[ Wraps the process of creating Python bindings for a target.
#
#    This function will create a CMake target "py_${npm_cxx_target}" from
#    the C++ source files in the provided source directory. The resulting
#    bindings will live in a shared library called "${npm_cxx_target}.so",
#    *i.e.* the C++ target with no "lib" prefix. As long as
#    "${npm_cxx_target}.so" is in your Python path, you should be able to load
#    the bindings.
#
#   :param npm_cxx_target: The name of the C++ target we are creating Python
#                          bindings for.
#   :param npm_src_dir: The full path to the directory containing the C++
#                       source code for the bindings.
#
#]]
function(nwx_pybind11_module npm_cxx_target npm_src_dir)
    file(GLOB_RECURSE _npm_py_files CONFIGURE_DEPENDS ${npm_src_dir}/*.cpp)

    set(_npm_py_target_name "py_${npm_cxx_target}")
    pybind11_add_module("${_npm_py_target_name}" MODULE ${_npm_py_files})
    target_link_libraries("${_npm_py_target_name}" PUBLIC ${npm_cxx_target})
    set_target_properties(
        "${_npm_py_target_name}"
        PROPERTIES LIBRARY_OUTPUT_NAME "${npm_cxx_target}"
    )
endfunction()

function(nwx_pybind11_tests npt_name npt_directory)
    if("${BUILD_TESTING}")
        add_test(
            NAME "${npt_name}"
            COMMAND "python" "${npt_directory}"
        )
        set_tests_properties(
            "${npt_name}"
            PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}"
        )
    endif()
endfunction()
