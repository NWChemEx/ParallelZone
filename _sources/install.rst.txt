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

#######################
Installing ParallelZone
#######################

ParallelZone uses `CMaize <https://cmakepp.github.io/CMaize/index.html>`__ as
its build system, more detailed build instructions can be found
`here <https://cmakepp.github.io/CMaize/getting_started/building/index.html>`__.

.. note::

   We strongly recommend users use CMake toolchains to pass arguments to the
   ``cmake`` command. Typically CMake toolchains live in files named
   ``<prefix>-toolchain.cmake``, where ``<prefix>-`` is an optional prefix to
   distinguish among toolchains. The contents of the CMake toolchain file
   usually just sets configuration values via a the standard CMake ``set``
   function, *i.e.*, ``set(<option_name> <value>)``.


.. todo::

   Figure out what's going on with CUDA, HIP, SYCL, and PAPI and update
   documentation accordingly. Right now all of these are off by default (and
   not used by ParallelZone) so users can safely ignore them.

***************
Install Command
***************

In many cases ParallelZone can be installed by:

.. code-block:: console

   git clone https://github.com/NWChemEx-Project/ParallelZone
   cd ParallelZone
   cmake -H. -Bbuild -DCMAKE_TOOLCHAIN_FILE=<path/to/toolchain/file>
   cmake --build build
   cmake --build build --target install

Where ``<path/to/toolchain/file>`` should be replaced by the path to your
toolchain file (absolute path is recommended).

.. todo::

   Can this be simplified by dropping the ``cmake --build build`` command?

*********************
Configuration Options
*********************

- ``BUILD_TESTING``. Off by default. Set to a truth-y value to enable testing.
- ``BUILD_PYBINDINGS``. On by default. When enabled the optional Python API is
  built.


*************************
ParallelZone Dependencies
*************************

If you have a problem building ParallelZone it's most likely because you
don't have a new enough version of CMake installed or because CMake can't find
one of ParallelZone's dependencies. This section enumerates ParallelZone's
dependencies.

C++ Compiler
============

ParallelZone relies on the C++17 standard and should work with any C++17
compliant compiler (GCC 9.x or newer). C++ compilers can often be installed by
your OS's package manager. CMake detects your C++ compiler via the value of
``CMAKE_CXX_COMPILER``. So ensure ``CMAKE_CXX_COMPILER`` is set to the C++
compiler you want to use.

.. todo::

   Clang support.

MPI
===

ParallelZone's heaviest dependency is MADNESS which implicitly brings in
:ref:`mpi`. Users must have an MPI distribution installed prior to building
ParallelZone. MPI distributions exist in most major OS-level package managers,
so check your package manager before installing MPI from source.


While CMake can find MPI if it is installed in your OS's standard spot, you may
need to give CMake a hint in some cases. MPI provides compiler wrappers and
it's best to let CMake discover MPI via the C++ wrapper. Add:

.. code-block:: cmake

   set(MPI_CXX_COMPILER <path/to/mpicxx>)

to your toolchain to point CMake to MPI's C++ compiler wrapper.

Cppyy
-----

URL: `<https://github.com/wlav/cppyy>`__

If Python bindings are enabled (controlled by the CMake variable
``BUILD_PYBINDINGS``, which is ``ON`` by default).  Cppyy is obtainable via
PyPI (*i.e.*, the ``pip`` command). If Python bindings are desired,
the python executable found by CMake must have Cppyy installed.


Other Dependencies
==================

The dependencies in this section can be built by ParallelZone's build system
when they are not located. Under normal circumstances users can ignore them.
They are listed here primarily for completeness.

Catch2
------

URL: `<https://github.com/catchorg/Catch2>`__

Used for unit testing. Only needed if unit testing is enabled (controlled by
the CMake variable ``BUILD_TESTING``, which is ``OFF`` by default).

Cereal
------

URL: `<https://github.com/USCiLab/cereal>`__

Used for serialization. The build system can automatically build Cereal for
you if it's not found.

CMaize
------

URL: `<https://cmakepp.github.io/CMaize/index.html>`__

Used to simplify writing a CMake-based build system. The build system will grab
it for you.
