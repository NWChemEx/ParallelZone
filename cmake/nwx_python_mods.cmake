# This function will skim a CMake target and create a file _init__.py that
# should be placed next to the shared library created by that target. This
# function assumes the target's:
#
# * public header files are in the ``PUBLIC_HEADER`` property
# * include paths are in the ``INTERFACE_INCLUDE_DIRECTORIES`` property
# * dependencies are targets and in the ``INTERFACE_LINK_LIBRAIRES`` property
#
#
# :Additional Named Arguments:
#     * NAMESPACE - The C++ namespace that your bindings live in. 
#     * DEPNAMESPACES - The C++ namespaceis that your bindings require,
#       i.e. previous Python package builds.
#     * PACKAGE - Package name to used as an alternative to NAMESPACE.  
#     * DEPENDS - List of modules this module depends on.
#
function(cppyy_make_python_package)
    #---------------------------------------------------------------------------
    #-----------------------Make sure we have cppyy installed-------------------
    #---------------------------------------------------------------------------
    if (NOT BUILD_PYBINDINGS)
        return()
    endif()
    #---------------------------------------------------------------------------
    #-----------------------Make sure we have cppyy installed-------------------
    #---------------------------------------------------------------------------
    find_package(Cppyy REQUIRED)
    #---------------------------------------------------------------------------
    #--------------------------Argument Parsing---------------------------------
    #---------------------------------------------------------------------------
    set(options MPI)
    set(oneValueArgs PACKAGE)
    set(multiValueArgs DEPENDS NAMESPACES DEPNAMESPACES)
    cmake_parse_arguments(install_data "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
    #---------------------------------------------------------------------------
    #--------------------------Get include directories--------------------------
    #---------------------------------------------------------------------------
    get_target_property(include_dirs ${install_data_PACKAGE} INTERFACE_INCLUDE_DIRECTORIES)
    foreach(item ${install_data_DEPENDS})
        get_target_property(include_item ${item} INTERFACE_INCLUDE_DIRECTORIES)
        list(APPEND include_dirs ${include_item})
    endforeach()
    if (install_data_MPI)
        list(APPEND include_dirs ${MPI_CXX_HEADER_DIR})
    endif()
    #---------------------------------------------------------------------------
    #--------------------------Get headers to includer--------------------------
    #---------------------------------------------------------------------------
    get_target_property(include_headers ${install_data_PACKAGE} PUBLIC_HEADER)
    get_filename_component(header_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
    #---------------------------------------------------------------------------
    #------------Collect the information we need off the target-----------------
    #---------------------------------------------------------------------------
    set(target_lib "$<TARGET_FILE_NAME:${install_data_PACKAGE}>")
    set(output_dir "${CMAKE_BINARY_DIR}/${install_data_PACKAGE}")
    #---------------------------------------------------------------------------
    #-----------------Generate _init__.py file contents------------------------
    #---------------------------------------------------------------------------
    set(init_file_name "${output_dir}/__init__.py")
    set(init_file "import cppyy\n\n")
    foreach(depnamespace ${install_data_DEPNAMESPACES})
        set(init_file "${init_file}from ${depnamespace} import \*\n")
    endforeach()
    set(init_file "${init_file}import os\n")
    set(init_file "${init_file}paths = list(set(\"${include_dirs}\".split(';')))\n")
    set(init_file "${init_file}for p in paths:\n")
    set(init_file "${init_file}    if p and p!=\"\":\n")
    set(init_file "${init_file}        cppyy.add_include_path(p)\n")
    #---------------------------------------------------------------------------
    #--Temporary band-aid for MADword MPI threads fixed in future cling/cppyy---
    #---------------------------------------------------------------------------
    if (install_data_MPI)
        set(init_file "${init_file}cppyy.cppdef(\"\"\"\\ \n")
        set(init_file "${init_file}\#define thread_local\n")
        set(init_file "${init_file}\#define is_server_thread \*_cling_is_server_thread()\n")
        set(init_file "${init_file}\#include \"${MADNESS_SOURCE_DIR}/src/madness/world/worldrmi.h\"\n")
        set(init_file "${init_file}\#undef thread_local\n")
        set(init_file "${init_file}\"\"\")\n")
    endif()
    set(init_file "${init_file}headers = \"${include_headers}\".split(';')\n")
    set(init_file "${init_file}for h in headers:\n")
    set(init_file "${init_file}    inc = os.path.join(\"${header_PREFIX}\",h)\n")
    set(init_file "${init_file}    cppyy.include(inc)\n")
    set(init_file "${init_file}\ncppyy.load_library(\"${CMAKE_BINARY_DIR}/${target_lib}\")\n\n")
    foreach(namespace ${install_data_NAMESPACES})
        set(init_file "${init_file}from cppyy.gbl import ${namespace}\n")
    endforeach()
    set(init_file "${init_file}from cppyy.gbl.std import array, vector, make_shared\n\n")
    #Write it out
    file(GENERATE OUTPUT ${init_file_name} CONTENT "${init_file}")
endfunction()
