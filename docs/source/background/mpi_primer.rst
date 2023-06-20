.. Copyright 2023 NWChemEx-Project
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

##########
MPI Primer
##########

:numref:`fig_mpi_runtime` illustrates a simplified version of how :ref:`mpi`
sees a program. The view is process-centric. When the program starts, MPI
defines a
set ``MPI_COMM_WORLD`` which includes all of program's processes. Within our
example ``MPI_COMM_WORLD`` has three processes. MPI assigns these processes an
integer value (ranging from 0 to "the number of processes minus 1") called the
process's rank. More complicated MPI setups can have further partition
``MPI_COMM_WORLD`` into subsets, but the overall point remains: MPI's view of
a program is a series of processes somehow grouped together.

.. _fig_mpi_runtime:

.. figure:: assets/mpi.png
   :align: center

   Illustration of MPI's runtime abstraction model.

In a typical MPI-based program parallelism is expressed by somehow mapping tasks
and data to ranks. For example say we have three arbitrary functions ``foo``,
``bar``, and ``baz` and a chunk of data we want to run ``foo``, ``bar``, and
``baz`` on. :numref:`fig_mpi_mappings` shows the three ways we can do this in
parallel.

.. _fig_mpi_mappings:

.. figure:: assets/mpi_mappings.png
   :align: center

   Possible ways of mapping data and tasks to MPI ranks.

In the :ref:`simd` approach depicted at the top of :numref:`fig_mpi_mappings` we
distribute the data over the MPI ranks and have each rank pass its local chunk
of data to the three functions. In :ref:`misd`, which is shown in the middle of
:numref:`fig_mpi_mappings` we instead distribute the functions over the ranks.
Finally, in the :ref:`mimd` model shown at the bottom of
:numref:`fig_mpi_mappings` we distribute both the data and the functions.

Unfortunately on today's machines it takes more than distributing tasks and/or
data to otherwise opaque ranks to achieve high-performance. This is because
the performance also depends on the hardware available to each rank and how
well utilized that hardware is.
