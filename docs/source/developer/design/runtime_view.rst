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

.. _runtime_view_design:

###########################
Designing RuntimeView Class
###########################

The need for the ``RuntimeView`` class grew out of
:ref:`parallel_runtime_design`. Here we flesh the design out more.

*************************************
Why Do We Need the RuntimeView Class?
*************************************

As discussed in :ref:`parallel_runtime_design`, the ``RuntimeView`` class is
ParallelZone's abstraction for modeling the runtime environment. ``RuntimeView``
will also serve as the top-level :ref:`api` for accessing ParallelZone
functionality.

*****************************************
What Should the RuntimeView Class Manage?
*****************************************

ParallelZone assumes it is managing the runtime environment for a potentially
multi-process program. In general that program can be running on a laptop, the
number one supercomputer in the world, or anything in between.

1. ``RuntimeView`` is a view of the runtime environment at the highest level.
   System-wide state will be accessible from ``RuntimeView``. State includes:

   - System-wide scheduling
   - System-wide printing
   - Processes
   - Hardware

#. Multi-process operations need to go through ``RuntimeView``.
#. MPI compatibility.
#. Flexibility of backend.
#. Setup/teardown of parallel resources

   - See :ref:`understanding_runtime_initialization_finalization` for more
     details, but basically we need callbacks.

************************
RuntimeView Architecture
************************

.. _fig_runtime_view:

.. figure:: assets/runtime_view.png
   :align: center

   Schematic illustration of the RuntimeView class and its major pieces.

The architecture of ``RuntimeView`` is shown in :numref:`fig_runtime_view`. This
addresses the above consideration by (numbering is from above):

1. Introducing components for each piece of state. Process to component affinity
   is maintained by ``ResourceSet`` objects.
#. Exposing MPI All-to-All operations at the ``RuntimeView`` level.

   - MPI All-to-One and One-to-All Operations are delegated to ``RAM`` and
     ``GPU`` objects in a particular ``ResourceSet``.
   - This facilitates selecting start/end points.

#. MPI support happens via the ``CommPP`` class.

#. The use of the PIMPL design allows us to hide many of the backend types. It
   also facilitates writing an implementation for a different backend down the
   line (although the API would need to change too).

#. Storing of callbacks allows us to tie the lifetime of the ``RuntimeView`` to
   the teardown of parallel resources, i.e., ``RuntimeView`` will automatically
   finalize any parallel resources which depend on ``RuntimeView`` before
   finalizing itself.

   - Note, finalization callbacks are stored in a stack to ensure a controlled
     teardown order as is usually needed for libraries with initialize/finalize
     functions.

Some finer points:

- The scheduler is envisioned as taking task graphs and scheduling them in a
  distributed and threaded manner. The latter relies on collaboration with
  ``ResourceSet`` instances.
- The Logger is an instance that every process can access. It represents the
  authoritative printing of the program. It's exact behavior should be
  customizable, but it is assumed this logger is always called in a SIMD manner
  by all processes. Default behavior is to redirect all output to ``/dev/null``
  except that of the root process.

*************
Proposed APIs
*************

Examples of all-to-all communications:

.. code-block:: c++

   auto rt = get_runtime();

   auto data = generate_data();

   // This is an all gather
   auto output = rt.gather(data);

   // This is an all reduce
   auto output2 = rt.reduce(data, op);


Example of tying another library's parallel runtime teardown to the lifetime of
a ``RuntimeView`` (note this is only relevant when ParallelZone starts MPI):

.. code-block:: c++

   // Create a RuntimeView object
   RuntimeView rt;

   // Initialize the other library
   other_library_initialize();

   // Register the corresponding finalization routine with the RuntimeView
   rt.stack_callback(other_library_finalize);

.. note::

   As written the APIs assume the data is going to/from RAM. If we eventually
   want to support other memory spaces we could create overloads which take the
   target space. In particular we note that we can NOT do things like:

   .. code-block:: c++

      auto output = rt.my_resource_set().ram().gather(data);

   because that would result in deadlock (it calls a series of all-to-one calls
   where each rank thinks it's the root).
