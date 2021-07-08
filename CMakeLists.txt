cmake_minimum_required(VERSION 3.14)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION)
project(runtime VERSION "${VERSION}" LANGUAGES CXX)

set(
    CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake"
    CACHE STRING "" FORCE
)

include(get_cpp)

### Options ###
option(BUILD_TESTING "Should we build the tests?" OFF)
option(BUILD_DOCS "Should we build the documentation?" OFF)
option(
    ONLY_BUILD_DOCS
    "If enabled and BUILD_DOCS is true no libraries will be built" OFF
)

option(BUILD_PYBINDINGS "Should we build Python3 bindings?" ON)

if("${BUILD_DOCS}")
    find_package(Doxygen REQUIRED)
    set(DOXYGEN_EXTRA_PACKAGES amsmath) # Extra LaTeX packages for documentation
    doxygen_add_docs(runtime_cxx_api "${CMAKE_CURRENT_LIST_DIR}/runtime")
    if("${ONLY_BUILD_DOCS}")
        return()
    endif()
endif()

cpp_find_or_build_dependency(
    madness
    URL github.com/m-a-d-n-e-s-s/madness
    VERSION 7ce06234c23aa8e0ab3d9e9b87eff9cd85390d80
    BUILD_TARGET MADworld
    FIND_TARGET MADworld
    CMAKE_ARGS BUILD_TESTING=OFF
               ENABLE_UNITTESTS=OFF
               MADNESS_BUILD_MADWORLD_ONLY=ON
               MADNESS_ENABLE_CEREAL=ON
)

# Note that hashes may differ with different compilers when typeid is used.
# Uncomment below to enable using typeid for hashes. 
# add_compile_definitions(BPHASH_USE_TYPEID)

cpp_find_or_build_dependency(
    bphash
    URL github.com/ryanmrichard/BPHash
    BUILD_TARGET bphash
    FIND_TARGET bphash::bphash
    BRANCH master
    CMAKE_ARGS BUILD_TESTING=OFF
)

cpp_add_library(
    runtime
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/runtime"
    INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include/runtime"
    DEPENDS MADworld bphash::bphash

)

if("${BUILD_TESTING}")
    cpp_find_or_build_dependency(
        Catch2
        URL github.com/catchorg/Catch2
        BUILD_TARGET Catch2
        FIND_TARGET Catch2::Catch2
		VERSION v2.13.3
    )
    cpp_add_tests(
        test_runtime
        SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/tests"
        INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/runtime"
        DEPENDS Catch2::Catch2 MADworld runtime
    )
endif()

