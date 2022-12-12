.. Copyright 2022 NWChemEx-Project
..
.. Licensed under the Apache License, Version 2.0 (the "License");
.. you may not use this file except in compliance with the License.
.. You may obtain a copy of the License at
..
.. http://www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.

##############################
ParallelZone Repository Layout
##############################

This page is meant to bring developers up to speed on what the various
directories and files are supposed to be used for.

*********************
Top-Level Directories
*********************

In alphabetical order the top-level directories in this repo are:

- ``.github/``. Workflows and assets needed for continuous integration using
  GitHub actions.
- ``cmake/``. CMake modules needed by the build system.
- ``docs/``. Contains source files and infrastructure for building
  ParallelZone's documentation.
- ``include/``. C++ header files exposing the public :ref:`api` of ParallelZone.
- ``src/``. C++ source files implementing the functionality exposed in the
  header files.
- ``tests/``. Source files and testing apparatuses for testing ParallelZone.

***************
Top-Level Files
***************

- ``.clang-format``. Settings used by the ``clang-format`` command to format
  the C++ files.
- ``.gitignore``. Tells ``git`` command which files and directories are not
  under version control.
- ``CMakeLists.txt``. Tells ``cmake`` command how to build ParallelZone.
- ``LICENSE``. The legal terms governing the use of ParallelZone.
- ``README.md``. The introduction/splash page people see when they go to the
  GitHub repo.


************************
docs/ Directory Contents
************************

- ``source/``. Contains the source files for the documentation. The internal
  layout reflects the intended layout of the documentation.
- ``Makefile``. Tells the ``make`` command how to build the documentation.
- ``README.md``. Instructions for building the documentation.
- ``requirements.txt``. Python dependencies needed to build the documentation.

*************************
tests/ Directory Contents
*************************

The tests are partitioned based on the types of tests.

- ``cxx``. Contain tests written in C++.
- ``cxx/unit_tests/``. Unit tests for the classes and functions written in C++.
- ``cxx/doc_snippets/``. The C++ code snippets used throughout the documentation.
  The snippets are literal included in the documentation and the testing
  mechanism ensures they stay up to date with code modifications.
- ``py``. Contains tests written in Python.
- ``py/unit_tests/``. Unit tests of the auto-generated Python APIs.
