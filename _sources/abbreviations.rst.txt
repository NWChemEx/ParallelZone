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


.. _abbreviations:

##########################################
Abbreviations Used Throughout ParallelZone
##########################################

The following subsections list (in alphabetical order) abbreviations encountered
throughout ParallelZone. Pages are encouraged to link to each abbreviation on
its first use.

.. _api:

***
API
***

Application Programming Interface. The literal functions you call to
programmatically interact with a software package.

.. _cpu:

***
CPU
***

Central processing unit.

.. _gpu:

***
GPU
***

Graphical processing unit.

.. _mimd:

****
MIMD
****

Multiple instructions, multiple data. A parallelism model in which each worker
executes different instructions on different data.

.. _misd:

****
MISD
****

Multiple instructions, single data. A parallelism model in which each worker
executes different instructions on the same data.

.. _mpi:

***
MPI
***

Message Passing Interface. An API standard defining functions and utilities
useful for writing software using distributed parallelism.

.. _simd:

****
SIMD
****

Single instruction, multiple data. Refers to a parallelism model in which each
parallel worker has different data, but executes the same function on that
data.

.. _sisd:

****
SISD
****

Single instruction, single data. Serial execution as expressed by the remaining
permutation of single/multiple instruction(s), single/multiple data.
