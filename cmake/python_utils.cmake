# Copyright 2025 NWChemEx-Project
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

include_guard()

#[[[
# Code factorization for determining python paths for NWChemEx repos.
#
# :param path: Variable which will hold the Python path.
# :type path: list[path]*
# :param kwargs: Keyword arguments to parse.
#
# **Keyword Arguments**
#
#   :keyword SUBMODULES: A list of other NWChemEx Python submodules which must
#                        be found in order for the test to run. For now, it is
#                        assumed that CMaize built the submodules, or they are
#                        installed in a place which is in the user's PYTHONPATH.
#]
#]]
function(nwx_python_path _npp_path)
    set(_npp_options "")
    set(_npp_one_val "")
    set(_npp_lists SUBMODULES)
    cmake_parse_arguments(
        "_npp" "${_npp_options}" "${_npp_one_val}" "${_npp_lists}" ${ARGN}
    )

    # N.B. This presently assumes we're building the Python submodules we
    #      need or they are present in the user's PYTHONPATH.
    message(DEBUG "Environment PYTHONPATH=$ENV{PYTHONPATH}")
    set(_npp_py_path "PYTHONPATH=$ENV{PYTHONPATH}")
    set(_npp_py_path "${_npp_py_path}:${CMAKE_BINARY_DIR}")
    if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/python")
        set(_npp_py_path "${_npp_py_path}:${CMAKE_SOURCE_DIR}/python")
    endif()
    foreach(_npp_submod ${_npp_SUBMODULES})
        # Add python source dir if it exists
        set(_npp_py_src_dir
            "${CMAKE_BINARY_DIR}/_deps/${_npp_submod}-src/src/python")
        if(IS_DIRECTORY "${_npp_py_src_dir}")
            set(_npp_py_path "${_npp_py_path}:${_npp_py_src_dir}")
        endif()
        # Add build dir if it exists
        set(_npp_sub_build_dir "${CMAKE_BINARY_DIR}/_deps/${_npp_submod}-build")
        if(IS_DIRECTORY "${_npp_sub_build_dir}")
            set(_npp_py_path "${_npp_py_path}:${_npp_sub_build_dir}")
        endif()
    endforeach()
    if(NOT "${NWX_PYTHON_EXTERNALS}" STREQUAL "")
        set(_npp_py_path "${_npp_py_path}:${NWX_PYTHON_EXTERNALS}")
    endif()
    message(DEBUG "Modified PYTHONPATH: ${_npp_py_path}")
    set("${_npp_path}" "${_npp_py_path}" PARENT_SCOPE)
endfunction()


#[[[ Wraps the process of registering Python-based tests with CTest
#
#   Calling this function will register a new Python-based test which can be
#   run with the CTest command.
#
#   This function assumes that your Python tests are governed by running a
#   Python module. More specifically it assumes that running a command like:
#
#   .. code-block::
#
#      python /path/to/some_module.py
#
#   will run your tests.
#
#   .. note::
#
#      The resulting test actually uses the Python interpreter that
#      ``nwx_pybind11_module`` found, *i.e.*, the raw ``python`` call is only
#      shown for clarity, not accuracy.
#
#   .. note::
#
#      This function assumes your test is a Python module, *i.e.*, a Python
#      script, and NOT a Python package, *i.e.*, a directory with an
#      ``__init__.py`` file.
#
#   :param name: The name for the test. This will be the name CTest
#                associates with the test.
#   :param driver: The name of the Python module responsible for driving
#                  the test. It is strongly recommended that you pass the
#                  full path to the Python module.
#
#   **Keyword Arguments**
#
#   :keyword SUBMODULES: A list of other NWChemEx Python submodules which must
#                        be found in order for the test to run. For now, it is
#                        assumed that CMaize built the submodules, or they are
#                        installed in a place which is in the user's PYTHONPATH.
#]]
function(nwx_pybind11_tests npt_name npt_driver)
    include(CTest)

    nwx_python_path(_npt_py_path ${ARGN})

    add_test(
        NAME "${npt_name}"
        COMMAND Python::Interpreter "${npt_driver}"
    )

    set_tests_properties(
        "${npt_name}"
        PROPERTIES ENVIRONMENT "${_npt_py_path}"
    )
endfunction()
