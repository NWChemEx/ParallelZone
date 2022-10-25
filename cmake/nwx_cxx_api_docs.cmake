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

include_guard()

option(BUILD_DOCS "Should we build the documentation?" OFF)
option(ONLY_BUILD_DOCS
      "If enabled and BUILD_DOCS is true no libraries will be built" OFF
)

macro(nwx_cxx_api_docs)
    if("${BUILD_DOCS}")
        find_package(Doxygen REQUIRED dot)

        # Extracts everything, whether it's documented or not
        SET(DOXYGEN_EXTRACT_ALL "YES")
        SET(DOXYGEN_EXTRACT_PRIVATE "YES")

        SET(DOXYGEN_USE_MDFILE_AS_MAINPAGE "README.md") # Reuse README.md

        SET(DOXYGEN_WARN_AS_ERROR "YES") # Crash build if there's a warning


        ########################################################################
        # Options for enabling full-featured UML diagrams
        ########################################################################
        SET(DOXYGEN_CLASS_GRAPH "YES")        # Show inheritance relations
        set(DOXYGEN_UML_LOOK "YES")           # Makes class diagrams into UML
        set(DOXYGEN_UML_DETAILS "YES")        # If "no", UML just shows names
        set(DOXYGEN_UML_LIMIT_NUM_FIELDS "0") # No limit on number of fields

        # If "no", undocumented entities show up in the UML diagrams
        SET(DOXYGEN_HIDE_UNDOC_RELATIONS "NO")

        # Shows relationship between template class and instances of class
        set(DOXYGEN_TEMPLATE_RELATIONS "YES")

        # Shows class implementation dependencies
        set(DOXYGEN_COLLABORATION_GRAPH "YES")

        ########################################################################
        # Settings for the files generated with dot tool
        ########################################################################
        set(DOXYGEN_HAVE_DOT "YES")            # Force Doxygen to use dot
        set(DOXYGEN_DOT_GRAPH_MAX_NODES "100") # Max # of nodes in a graph
        set(DOXYGEN_MAX_DOT_GRAPH_DEPTH  "0")  # Max # of nestings in graph
        set(DOXYGEN_DOT_IMAGE_FORMAT "SVG")
        set(DOXYGEN_INTERACTIVE_SVG "YES")
        set(DOXYGEN_DOT_TRANSPARENT "YES")

        # Extra LaTeX packages
        set(DOXYGEN_EXTRA_PACKAGES amsmath amsfonts)

        # Make a target for the C++ documentation by mangling the name
        string(TOLOWER "${PROJECT_NAME}" _ncad_lc_name)
        set(_ncad_target "${_ncad_lc_name}_cxx_api")

        # Actually run Doxygen, and exit if we're only building documentation
        doxygen_add_docs("${_ncad_target}" ${ARGV} ALL)
        if("${ONLY_BUILD_DOCS}")
            return()
        endif()
    endif()
endmacro()
