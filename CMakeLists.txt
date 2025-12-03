# Copyright 2022 NWChemEx-Project
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

cmake_minimum_required(VERSION 3.14)

#Sets the version to whatever git thinks it is
include(cmake/get_version_from_git.cmake)
get_version_from_git(parallelzone_version "${CMAKE_CURRENT_LIST_DIR}")
project(parallelzone VERSION "${parallelzone_version}" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include(cmake/get_cmaize.cmake)

# Work out the project paths
set(project_inc_dir "${CMAKE_CURRENT_LIST_DIR}/cpp/include/${PROJECT_NAME}")
set(project_src_dir "${CMAKE_CURRENT_LIST_DIR}/cpp/src/${PROJECT_NAME}")

# Builds C++ API documentation
include(cmake/nwx_cxx_api_docs.cmake)
nwx_cxx_api_docs("README.md" "${project_inc_dir}" "${project_src_dir}")

### Options ###
cmaize_option_list(
    BUILD_TESTING OFF "Should we build the tests?"
    BUILD_PYBIND11_PYBINDINGS ON "Should we build pybind11 python bindings?"
    BUILD_CPP_JOULES OFF "Enable energy usage tracking with CPP Joules library?"
)

### Dependendencies ###
find_package(MPI REQUIRED)
set(project_depends "MPI::MPI_CXX")

cmaize_find_or_build_dependency(
    cereal
    URL github.com/USCiLab/cereal
    VERSION v1.3.0
    BUILD_TARGET cereal
    FIND_TARGET cereal
    CMAKE_ARGS JUST_INSTALL_CEREAL=ON
)
list(APPEND project_depends cereal)

cmaize_find_or_build_dependency(
    spdlog
    URL github.com/gabime/spdlog
    VERSION ad0e89cbfb4d0c1ce4d097e134eb7be67baebb36
    BUILD_TARGET spdlog
    FIND_TARGET spdlog::spdlog
    CMAKE_ARGS SPDLOG_INSTALL=ON
)
list(APPEND project_depends spdlog)

if("${BUILD_CPP_JOULES}")
    cmaize_find_or_build_dependency(
        cpp_joules
        BUILD_CPP_JOULES
        URL github.com/rishalab/CPPJoules
        VERSION main
        BUILD_TARGET CPP_Joules
        FIND_TARGET CPP_Joules::CPP_Joules
    )
    list(APPEND project_depends cpp_joules)
endif()

find_package(Python COMPONENTS Interpreter Development)

cmaize_find_or_build_dependency(
    pybind11
    URL github.com/pybind/pybind11
    BUILD_TARGET pybind11::module
    FIND_TARGET pybind11::module
    CMAKE_ARGS PYBIND11_INSTALL=ON
               PYBIND11_FINDPYTHON=ON
)

cmaize_add_library(
    ${PROJECT_NAME}
    SOURCE_DIR "${project_src_dir}"
    INCLUDE_DIRS "${project_inc_dir}"
    DEPENDS "${project_depends}"
)

target_link_libraries(
    "${PROJECT_NAME}" PUBLIC pybind11::module Python::Python
)

if("${BUILD_CPP_JOULES}")
    target_compile_definitions("${PROJECT_NAME}" PRIVATE BUILD_CPP_JOULES)
endif()

if("${BUILD_TESTING}")
    set(CXX_TEST_DIR "${CMAKE_CURRENT_LIST_DIR}/tests/cxx")
    set(PYTHON_TEST_DIR "${CMAKE_CURRENT_LIST_DIR}/tests/python")

    cmaize_find_or_build_dependency(
        Catch2
        URL github.com/catchorg/Catch2
        BUILD_TARGET Catch2
        FIND_TARGET Catch2::Catch2
        VERSION v3.6.0
    )

    cmaize_add_tests(
        test_unit_parallelzone
        SOURCE_DIR "${CXX_TEST_DIR}/unit_tests"
        INCLUDE_DIRS "${project_src_dir}"
	    DEPENDS Catch2::Catch2 ${PROJECT_NAME}
    )

    cmaize_add_tests(
        test_parallelzone_docs
        SOURCE_DIR "${CXX_TEST_DIR}/doc_snippets"
        INCLUDE_DIRS "${project_src_dir}"
        DEPENDS Catch2::Catch2 ${PROJECT_NAME}
    )

    add_test(
        NAME "test_pz_under_mpi"
        COMMAND "${MPIEXEC_EXECUTABLE}" "${MPIEXEC_NUMPROC_FLAG}" "2"
               "${CMAKE_BINARY_DIR}/test_unit_parallelzone"
    )

    add_test(
        NAME "test_pz_docs_under_mpi"
        COMMAND "${MPIEXEC_EXECUTABLE}" "${MPIEXEC_NUMPROC_FLAG}" "2"
                "${CMAKE_BINARY_DIR}/test_parallelzone_docs"
    )

    include(cmake/python_utils.cmake)
    nwx_pybind11_tests(
        py_parallelzone "${PYTHON_TEST_DIR}/unit_tests/test_parallelzone.py"
    )

    nwx_pybind11_tests(
        py_doc_snippets "${PYTHON_TEST_DIR}/doc_snippets/test_doc_snippets.py"
    )
endif()

# Install the python package
cmake_path(GET Python_EXECUTABLE PARENT_PATH pyroot)
cmake_path(GET pyroot PARENT_PATH pyroot)
cmake_path(RELATIVE_PATH Python_SITELIB BASE_DIRECTORY "${pyroot}" OUTPUT_VARIABLE pysitelib)
install(
    DIRECTORY "${CMAKE_SOURCE_DIR}/python/${PROJECT_NAME}"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/${pysitelib}"
)

cmaize_add_package(${PROJECT_NAME} NAMESPACE nwx::)
