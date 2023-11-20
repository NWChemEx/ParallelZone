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
Using ParallelZone with mpi4py
##############################

.. note::

   mpi4py is not installed by ParallelZone, so should be installed separately.
   See `mpi4py documentation
   <https://mpi4py.readthedocs.io/en/stable/install.html>`__ for more
   information.

.. warning::

   Python support for ParallelZone is currently in beta. APIs are not
   considered stable at this time and are subject to change.

mpi4py is the most popular package that provides Python bindings for the MPI
standard. Here, we describe how you can use mpi4py along with ParallelZone.

**************************************
MPI Initialization and Finalization
**************************************

The MPI standard requires `MPI_Init` or `MPI_Init_thread` function to be called
by each process to initialize the MPI execution environment. mpi4py performs
initialization (if necessary)
automatically when `MPI` module is imported with `from mpi4py import MPI`.
By default thread support is enabled with `MPI_THREAD_MULTIPLE` option, which is
also the required thread support level for ParallelZone. If
MPI is already initialized, i.e. by ParallelZone, mpi4py basically uses the
same MPI environment. Finalization is also handled automatically by mpi4py
when Python processes exit.

Therefore, for a general use case, where `MPI_COMM_WORLD` refers to the same
set of resources for mpi4py and ParallelZone, they are compatible with each
other.

.. note::

   It is possible to disable automatic initialization of mpi4py or change the
   thread support level manually. See the `mpi4py documentation
   <https://mpi4py.readthedocs.io/en/stable/mpi4py.html?highlight=initialization#runtime-configuration-options>`__
   for more details. However, note that it is not possible to pass a
   communicator to mpi4py explicitly.

************************************************
Passing mpi4py Communicator to ParallelZone
************************************************

If a user is interested in only sharing a subset of resources with ParallelZone,
then the user needs to initialize ParallelZone by passing the communicator
created by mpi4py. An example, where the user passes `MPI_COMM_SELF` as the
communicator for ParallelZone is given below.

.. code-block::

   from mpi4py import MPI
   import cppyy.ll

   pz_comm = cppyy.ll.cast["MPI_Comm"](MPI._handleof(MPI.COMM_SELF))
   pz_world = parallelzone.runtime.RuntimeView(pz_comm)
