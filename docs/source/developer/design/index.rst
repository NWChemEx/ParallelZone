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

####################
Design Documentation
####################

Topics in this section document the design considerations which went into
developing ParallelZone.


*****************
High-Level Design
*****************

.. toctree::
   :maxdepth: 1

   parallel_zone
   architecture


***********************
Parallel Runtime Design
***********************

The "parallel runtime" component is the part of ParallelZone tasked with
facilitating parallel programming. The topics in this part dive into design
details of the pieces of the parallel runtime component.

.. toctree::
   :maxdepth: 1

   parallel_runtime
   runtime_view
   resource_set
   ram

MPI Helpers
===========

The "MPI Helpers" component of ParallelZone is a sub-component of the
parallel runtime component. The MPI Helpers component provides low-level,
object-oriented APIs for interacting with MPI.

.. toctree::
   :maxdepth: 1

   compp

****************
Utilities Design
****************

.. toctree::
   :maxdepth: 1

   hashing
   serialization
   reflection
