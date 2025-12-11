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

# Downloads common CMake modules used throughout NWChemEx
include(cmake/get_nwx_cmake.cmake)

#Sets the version to whatever git thinks it is
include(get_version_from_git)
get_version_from_git(parallelzone_version "${CMAKE_CURRENT_LIST_DIR}")
project(parallelzone VERSION "${parallelzone_version}" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include(get_cmaize)

# Work out the project paths
set(project_inc_dir "${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}")
set(project_src_dir "${CMAKE_CURRENT_LIST_DIR}/src/${PROJECT_NAME}")

# Builds C++ API documentation
include(nwx_cxx_api_docs)
nwx_cxx_api_docs("README.md" "${project_inc_dir}" "${project_src_dir}")

### Options ###
cmaize_option_list(
    BUILD_TESTING OFF "Should we build the tests?"
    BUILD_PYBIND11_PYBINDINGS ON "Should we build pybind11 python bindings?"
    BUILD_CPP_JOULES OFF "Enable energy usage tracking with CPP Joules library?"
    BUILD_CUDA_BINDINGS OFF "Enable CUDA Bindings"
    BUILD_HIP_BINDINGS  OFF "Enable HIP Bindings"
    BUILD_SYCL_BINDINGS OFF "Enable SYCL Bindings"
    BUILD_PAPI_BINDINGS OFF "Enable PAPI Bindings"
)

if (BUILD_CUDA_BINDING OR BUILD_HIP_BINDINGS OR BUILD_SYCL_BINDING)
  include(build_device)
endif()

### Dependendencies ###
set(project_depends "MPI::MPI_CXX")

find_package(MPI REQUIRED)

cmaize_find_or_build_dependency(
    spdlog
    URL github.com/gabime/spdlog
    VERSION v1.16.0
    BUILD_TARGET spdlog
    FIND_TARGET spdlog::spdlog
    CMAKE_ARGS SPDLOG_INSTALL=ON
)
list(APPEND project_depends spdlog)

# PAPI bindings are enabled, leading to building PAPI with CUDA or rocm support
if("${BUILD_PAPI_BINDINGS}")
    include(build_papi)
endif ()

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

cmaize_find_or_build_dependency(
    cereal
    URL github.com/USCiLab/cereal
    VERSION v1.3.2
    BUILD_TARGET cereal
    FIND_TARGET cereal::cereal
    CMAKE_ARGS JUST_INSTALL_CEREAL=ON
)
list(APPEND project_depends cereal)

cmaize_add_library(
    ${PROJECT_NAME}
    SOURCE_DIR "${project_src_dir}"
    INCLUDE_DIRS "${project_inc_dir}"
    DEPENDS "${project_depends}"
)

if("${BUILD_CPP_JOULES}")
    target_compile_definitions("${PROJECT_NAME}" PRIVATE BUILD_CPP_JOULES)
endif()

# N.B. this is a no-op if BUILD_PYBIND11_PYBINDINGS is not turned on
include(nwx_pybind11)
nwx_add_pybind11_module(
    ${PROJECT_NAME}
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/python"
    DEPENDS "${PROJECT_NAME}"
)

if("${BUILD_TESTING}")
    set(CXX_TEST_DIR "${CMAKE_CURRENT_LIST_DIR}/tests/cxx")
    set(PYTHON_TEST_DIR "${CMAKE_CURRENT_LIST_DIR}/tests/python")

    include(get_catch2)

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

    # N.B. these are no-ops if BUILD_PYBIND11_PYBINDINGS is not turned on
    nwx_pybind11_tests(
        py_parallelzone "${PYTHON_TEST_DIR}/unit_tests/test_parallelzone.py"
    )

    nwx_pybind11_tests(
        py_doc_snippets "${PYTHON_TEST_DIR}/doc_snippets/test_doc_snippets.py"
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
endif()

cmaize_add_package(${PROJECT_NAME} NAMESPACE nwx::)
