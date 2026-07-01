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

# Put the shared NWXCMake modules on CMAKE_MODULE_PATH *before* project() so
# nwx_set_version() is available to compute the project version.
include(cmake/get_nwx_cmake.cmake)

include(nwx_set_version)
nwx_set_version(parallelzone_version "${CMAKE_CURRENT_LIST_DIR}")
project(parallelzone VERSION "${parallelzone_version}" LANGUAGES CXX)

include(disable_in_source_builds)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

### Options ###
option(BUILD_TESTING "Should we build the tests?" OFF)
option(BUILD_PYBIND11_BINDINGS "Should we build pybind11 python bindings?" ON)
option(BUILD_CPP_JOULES "Enable energy usage tracking with CPP Joules library?" OFF)
option(BUILD_CUDA_BINDINGS "Enable CUDA Bindings" OFF)
option(BUILD_HIP_BINDINGS  "Enable HIP Bindings"  OFF)
option(BUILD_SYCL_BINDINGS "Enable SYCL Bindings" OFF)
option(BUILD_PAPI_BINDINGS "Enable PAPI Bindings" OFF)

# Project paths (relative dirs are resolved against CMAKE_CURRENT_SOURCE_DIR by
# the NWXCMake helpers). project_priv_dir is the private source root the C++
# tests include detail_ headers from as <parallelzone/.../detail_/...>.
set(project_inc_dir "cxx/include")
set(project_src_dir "cxx/src")
set(project_priv_dir "${CMAKE_CURRENT_LIST_DIR}/cxx/src")

# Builds C++ API documentation (no-op unless BUILD_DOCS is on)
include(nwx_cxx_api_docs)
nwx_cxx_api_docs("README.md" "${project_inc_dir}" "${project_src_dir}")

if(BUILD_CUDA_BINDINGS OR BUILD_HIP_BINDINGS OR BUILD_SYCL_BINDINGS)
    include(build_device)
endif()

### Dependencies ###
include(nwx_find_mpi)
nwx_find_mpi()

include(get_dependencies)
get_dependencies(spdlog cereal)

set(project_depends MPI::MPI_CXX spdlog::spdlog cereal::cereal)

# PAPI bindings are enabled, leading to building PAPI with CUDA or ROCm support
if(BUILD_PAPI_BINDINGS)
    include(build_papi)
endif()

if(BUILD_CPP_JOULES)
    include(FetchContent)
    FetchContent_Declare(
        cpp_joules
        GIT_REPOSITORY https://github.com/rishalab/CPPJoules
        GIT_TAG main
    )
    FetchContent_MakeAvailable(cpp_joules)
    list(APPEND project_depends CPP_Joules::CPP_Joules)
endif()

### Library ###
include(nwx_library)
nwx_library(
    ${PROJECT_NAME} "${project_inc_dir}" "${project_src_dir}" ${project_depends}
)

if(BUILD_CPP_JOULES)
    target_compile_definitions(${PROJECT_NAME} PRIVATE BUILD_CPP_JOULES)
endif()

### Python bindings (no-op unless BUILD_PYBIND11_BINDINGS is on) ###
include(nwx_python_module)
nwx_python_module(${PROJECT_NAME} "${project_src_dir}")

### Tests ###
if(BUILD_TESTING)
    include(catch2_tests_from_dir)
    catch2_tests_from_dir(
        test_unit_parallelzone "tests/cxx/unit_tests" ${PROJECT_NAME}
    )
    catch2_tests_from_dir(
        test_parallelzone_docs "tests/cxx/doc_snippets" ${PROJECT_NAME}
    )
    # Tests reach into the private source tree for detail_ headers.
    target_include_directories(test_unit_parallelzone PRIVATE "${project_priv_dir}")
    target_include_directories(test_parallelzone_docs PRIVATE "${project_priv_dir}")

    # Also run the C++ tests under MPI with 2 ranks.
    include(nwx_mpi_test)
    #nwx_mpi_test(test_pz_under_mpi      test_unit_parallelzone NPROC 2)
    #nwx_mpi_test(test_pz_docs_under_mpi test_parallelzone_docs NPROC 2)

    # Python tests (no-op unless BUILD_PYBIND11_BINDINGS is on)
    include(nwx_python_test)
    nwx_python_test(
        py_parallelzone
        "${CMAKE_CURRENT_LIST_DIR}/tests/python/unit_tests/test_parallelzone.py"
    )
    nwx_python_test(
        py_doc_snippets
        "${CMAKE_CURRENT_LIST_DIR}/tests/python/doc_snippets/test_doc_snippets.py"
    )
endif()
