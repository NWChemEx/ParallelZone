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
#TODO: Get from git
set(VERSION 1.0)
project(parallelzone VERSION "${VERSION}" LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
        nwx_cmake
        GIT_REPOSITORY https://github.com/NWChemEx-Project/NWXCMake
)
FetchContent_MakeAvailable(nwx_cmake)
list(APPEND CMAKE_MODULE_PATH "${nwx_cmake_SOURCE_DIR}/cmake")

set(
    CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake"
    CACHE STRING "" FORCE
)

include(get_cpp)
include(nwx_cxx_api_docs)


### Options ###
option(BUILD_TESTING "Should we build the tests?" OFF)
option(BUILD_PYBINDINGS "Should we build Python3 bindings?" ON)
option(BUILD_CUDA_BINDINGS "Enable CUDA Bindings" OFF )
option(BUILD_HIP_BINDINGS  "Enable HIP Bindings"  OFF )
option(BUILD_SYCL_BINDINGS "Enable SYCL Bindings" OFF )
option(BUILD_PAPI_BINDINGS "Enable PAPI Bindings" OFF )

if (BUILD_CUDA_BINDING OR BUILD_HIP_BINDINGS OR BUILD_SYCL_BINDING)
  include(build_device)
endif()

# Work out the project paths
set(project_inc_dir "${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}")
set(project_src_dir "${CMAKE_CURRENT_LIST_DIR}/src/${PROJECT_NAME}")

# Project dependencies
set(project_depends "")

# set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/docs/build/html/cxx_api)
nwx_cxx_api_docs("README.md" "${project_inc_dir}" "${project_src_dir}")

#TODO: Make sure MADworld uses the Cereal we build
cpp_find_or_build_dependency(
    MADworld
    URL github.com/m-a-d-n-e-s-s/madness
    #VERSION 20d4d7cffbbc98dd7b7d6a5feb7d68f1df067cd6
    VERSION 997e8b458c4234fb6c8c2781a5df59cb14b7e700
    BUILD_TARGET MADworld
    FIND_TARGET MADworld
    CMAKE_ARGS ENABLE_UNITTESTS=OFF
               MADNESS_BUILD_MADWORLD_ONLY=ON
               MADNESS_ENABLE_CEREAL=ON
               ENABLE_MKL=OFF
               ENABLE_ACML=OFF
               BUILD_TESTING=OFF
)

list(APPEND project_depends MADworld)

cpp_find_or_build_dependency(
    spdlog
    URL github.com/gabime/spdlog
    VERSION ad0e89cbfb4d0c1ce4d097e134eb7be67baebb36
    BUILD_TARGET spdlog
    FIND_TARGET spdlog::spdlog
)
list(APPEND project_depends spdlog::spdlog)

# PAPI bindings are enabled, leading to building PAPI with CUDA or rocm support
if("${BUILD_PAPI_BINDINGS}")
    include(build_papi)
endif ()


cpp_find_or_build_dependency(
    cereal
    URL github.com/USCiLab/cereal
    BUILD_TARGET cereal
    FIND_TARGET cereal::cereal
    CMAKE_ARGS JUST_INSTALL_CEREAL=ON
)
list(APPEND project_depends cereal::cereal)

cpp_add_library(
    ${PROJECT_NAME}
    SOURCE_DIR "${project_src_dir}"
    INCLUDE_DIR "${project_inc_dir}"
    DEPENDS "${project_depends}"
)

if("${BUILD_TESTING}")
    set(CXX_TEST_DIR "${CMAKE_CURRENT_LIST_DIR}/tests/cxx")

    cpp_find_or_build_dependency(
        Catch2
        URL github.com/catchorg/Catch2
        BUILD_TARGET Catch2
        FIND_TARGET Catch2::Catch2
		VERSION v2.13.9
    )
    cpp_add_tests(
        test_parallelzone
        SOURCE_DIR "${CXX_TEST_DIR}/unit_tests"
        INCLUDE_DIR "${project_src_dir}"
        DEPENDS Catch2::Catch2 ${PROJECT_NAME}
    )
    cpp_add_tests(
        test_parallelzone_docs
        SOURCE_DIR "${CXX_TEST_DIR}/doc_snippets"
        INCLUDE_DIR "${project_src_dir}"
        DEPENDS Catch2::Catch2 ${PROJECT_NAME}
    )

    find_package(MPI REQUIRED)
    add_test(
        NAME "test_pz_under_mpi"
        COMMAND "${MPIEXEC_EXECUTABLE}" "${MPIEXEC_NUMPROC_FLAG}" "2"
               "${CMAKE_BINARY_DIR}/test_parallelzone"
    )
    add_test(
        NAME "test_pz_docs_under_mpi"
        COMMAND "${MPIEXEC_EXECUTABLE}" "${MPIEXEC_NUMPROC_FLAG}" "2"
                "${CMAKE_BINARY_DIR}/test_parallelzone_docs"
    )
endif()

if("${BUILD_PYBINDINGS}")
    # Cppyy bindings
    #include(nwx_python_mods)
    #cppyy_make_python_package(MPI PACKAGE parallelzone NAMESPACES parallelzone)

    cpp_find_or_build_dependency(
        pybind11
        URL github.com/pybind/pybind11
        BUILD_TARGET pybind11::pybind11
        FIND_TARGET pybind11::embed
        CMAKE_ARGS PYBIND11_INSTALL=ON
                   PYBIND11_FINDPYTHON=ON
    )
    file(
        GLOB_RECURSE PY_FILES
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/src/python/*.cpp
    )
    pybind11_add_module(py_parallelzone MODULE ${PY_FILES})
    target_link_libraries(py_parallelzone PUBLIC ${PROJECT_NAME})
    set_target_properties(
        py_parallelzone
        PROPERTIES LIBRARY_OUTPUT_NAME
        parallelzone
    )

    if("${BUILD_TESTING}")
        set(PY_TEST_DIR "${CMAKE_CURRENT_LIST_DIR}/tests/python")
        add_test(
            NAME "pytest_parallelzone"
            COMMAND "python" "${PY_TEST_DIR}/module.py"
        )
    endif()
endif()
