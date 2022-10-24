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

.. _parallel_runtime_design:

#########################################
Designing ParallelZone's Parallel Runtime
#########################################

:ref:`parallel_zone_design` described the overall design points which went into
the architecture of ParallelZone. Here we focus on the architecture of the
parallel runtime component.

**********************************
Why Do We Need a Parallel Runtime?
**********************************

The primary goal of ParallelZone is to be a runtime system for parallel
software. To accomplish this ParallelZone must have abstractions the software
can use to interact with the runtime environment in a parallel manner. Exactly
what these interactions entail is defined below.

*********************
Design Considerations
*********************

The parallel runtime will be responsible for addressing many of the
considerations raised in the :ref:`parallel_zone_design` section. Including:

- Object-oriented (both the parallel runtime itself and the data it operates on)
- Support for process, CPU-threading, and GPU-threading
- Extendable if/when new hardware parallelism arises (e.g. tensor cores)
- Support for MPI
- Scheduling of tasks

Additionally, we recognize that the parallel runtime will:

- need to interact with all of the other components of ParallelZone (hardware,
  logger, and utilities)
- manage the lifetime of parallel resources (think ``MPI_initialize``)
- track resource usage
- logical partitioning
- needs to understand locality/affinity of hardware
