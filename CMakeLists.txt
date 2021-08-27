cmake_minimum_required(VERSION 3.14)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION)
project(parallelzone VERSION "${VERSION}" LANGUAGES CXX)

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
    doxygen_add_docs(parallelzone_cxx_api "${CMAKE_CURRENT_LIST_DIR}/parallelzone")
    if("${ONLY_BUILD_DOCS}")
        return()
    endif()
endif()

cpp_find_or_build_dependency(
    cereal
    URL github.com/USCiLab/cereal
    BUILD_TARGET cereal
    FIND_TARGET cereal::cereal
    CMAKE_ARGS JUST_INSTALL_CEREAL=ON
)

#TODO: Make sure MADworld uses the Cereal we build
cpp_find_or_build_dependency(
    MADworld
    URL github.com/m-a-d-n-e-s-s/madness
    VERSION 7ce06234c23aa8e0ab3d9e9b87eff9cd85390d80
    BUILD_TARGET MADworld
    FIND_TARGET MADworld
    CMAKE_ARGS ENABLE_UNITTESTS=OFF
               MADNESS_BUILD_MADWORLD_ONLY=ON
               MADNESS_ENABLE_CEREAL=ON
               ENABLE_MKL=OFF
               ENABLE_ACML=OFF
               BUILD_TESTING=OFF
    DEPENDS cereal	       
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
    parallelzone
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/parallelzone"
    INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include/parallelzone"
    DEPENDS MADworld bphash::bphash cereal

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
        test_parallelzone
        SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/tests"
        INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/parallelzone"
        DEPENDS Catch2::Catch2 MADworld parallelzone
    )
endif()

