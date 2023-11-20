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
can use to interact with the runtime environment in a parallel manner. Ideally
these interactions should make the task of writing parallel code as simple as
possible.

*********************
Design Considerations
*********************

The parallel runtime will be responsible for addressing many of the
considerations raised in the :ref:`parallel_zone_design` section. Including:

1. Object-oriented (both the parallel runtime itself and the data it operates
   on)
#. Support for process, CPU-threading, and GPU-threading
#. Extendable if/when new hardware parallelism arises (e.g. tensor cores)
#. Support for MPI
#. Scheduling of tasks

Additionally, we recognize that the parallel runtime will:

6. manage the lifetime of parallel resources (think ``MPI_initialize``)
#. track resource usage
#. logical partitioning
#. needs to understand locality/affinity of hardware
#. compatible with not being the top (*i.e.*, should work even when it's not
   wrapping ``MPI_COMM_WORLD``).

***********************
Parallel Runtime Design
***********************

.. _fig_parallel_runtime:

.. figure:: assets/parallel_runtime_design.png
   :align: center

   High-level architecture of the parallel runtime component of ParallelZone.

:numref:`fig_parallel_runtime` provides a high-level overview of the parallel
runtime in ParallelZone. The ``RuntimeView`` class is an abstraction which
models the runtime environment. The ``ResourceSet`` class is used to group
resources into affinity sets. The various hardware classes (*e.g.*, ``CPU``,
``RAM``, etc.) and the ``Scheduler`` instances are still considered opaque
entities for the purposes of this page.

RuntimeView
===========

Full discussion :ref:`runtime_view_design`.

As the name suggests ``RuntimeView`` is a view of the actual runtime
environment. This means it does not own the resources it accesses. Similarly,
copying ``RuntimeView`` instances do not create more resources, instead it just
makes another view to the same set of resources. Conceptually, ``RuntimeView``
is a container filled with ``ResourceSet`` objects.

The ``RuntimeView`` class directly addresses (numbers refer to above):

1. ``RuntimeView`` is an object, methods in the interface assume they interact
   with objects.
#. Processes map to ``ResourceSet`` objects, so managing ``ResourceSet`` objects
   is a large part of supporting process parallelism.

4. Ultimately powered by MPI
#. Responsible for managing tasks which use more than one process.
#. Reference counting of ``RuntimeView`` used to manage MPI lifetime
#. Tracks resource usage above the process level.
#. Supports logical partitioning at or above the process level.
#. Responsible for the overall mapping of hardware to ``ResourceSet``
#. State is tied to underlying MPI communicator. Splitting the communicator is
   allowed.

ResourceSet
===========

Full discussion is at :ref:`resource_set_design`.

Each process is associated with a ``ResourceSet`` instance. The ``ResourceSet``
affiliated with the current process contains the resources which are local
and directly accessible to that process.

1. It's an object, uses objects to model resources, and assumes it will interact
   with objects.
#. Managing parallelism of CPU/GPU threads handled by sub-objects.
#. Additional members can be added for new hardware.
#. Data movement from one ``ResourceSet`` to another uses MPI.
#. Responsible for managing tasks below the process level.
#. Tracks resource usage below process level
#. Allows logical partitioning of local resources
#. Understands that its resources are local and resources in other
   ``ResourceSet`` objects are in general local.

Hardware Classes
================

See discussions at :ref:`ram_design`.

These classes have not been designed in detail yet. For now we note they should
allow the user to query whatever information they want in order to make
informed runtime decisions. They also are responsible for encapsulating the
mechanisms for interacting with that device (*e.g.*, sending result from the
memory of one ``ResourceSet`` to that of another).

Scheduler Classes
=================

The scheduler has not been designed yet.

******************
Additional Options
******************

Ultimately the runtime environment is a singleton. It may make more sense to
actually create a class ``Runtime`` which holds the state of the runtime
environment and have ``RuntimeView`` really hold a reference to the ``Runtime``
object. The primary benefit of this is that it better maps to MPI (which really
is a singleton).
