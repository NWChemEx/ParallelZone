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
#. MPI compatability.
#. Flexibility of backend.

***************************************************************
How Does RuntimeView Manage the Parallel Computing Environment?
***************************************************************

.. _design_parallel_manage:

Currently when a RuntimeView object is created, the standard MPI function 
``MPI_Init()`` is called to initialize the parallel computing 
environment. One legacy of different NWChemEx repositories is that many 
repositories have their own functions to initialize and finalize the parallel 
computing environment. These functions may rely on some external packages, such
as MADNESS we used to use in ParallelZone, to manage the paralell environment.
Different repositories depending on common external packages makes the 
dependency relation complicated and could create a diamond dependency problem. 
Installing the correct versions of package may be very tricky and building all
NWChemEx repositories would be very chanllenging. To make our lives easier we 
want every NWChemEx repository which is downstream to ParallelZone to use 
ParallelZone for parallel environment management.

In order to provide a common API to other downstream repositories for parallel 
environment management, callback functions are implemented in the 
``RuntimeView`` class to finalize the parallel environment. A function stack 
member is put into the ``RuntimeView`` class to store the callbacks. A 
downstream repository can register its version of finalize functions as 
finalize callbacks into this stack. When a calculation is done, the destructor
of the ``RuntimeView`` object is called, and in the destructor the finalize 
registered callbacks in the stack are called in a last-in-first-out fashion.

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

#. MPI_Init() is called directly to initialize the parallel computing 
   environment. Callbacks are implemented to allow different finalize function
   to clean the parallel computing environment. For the design considerations 
   on callbacks please see :ref:`design_parallel_manage`.

#. The use of the PIMPL design allows us to hide many of the backend types. It
   also facilitates writing an implementation for a different backend down the
   line (although the API would need to change too).

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


An example of using callbacks to finalize the parallel computing environment:

.. code-block:: c++

   // Create a RuntimeView object
   RuntimeView rt;

   // Register the corresponding finalize() function into the callback stack
   rt.stack_callback(your_finalize_function());

The designated function ``your_finalize_function()`` would be called when the
RuntimeView object is destroyed.

.. note::

   As written the APIs assume the data is going to/from RAM. If we eventually
   want to support other memory spaces we could create overloads which take the
   target space. In particular we note that we can NOT do things like:

   .. code-block:: c++

      auto output = rt.my_resource_set().ram().gather(data);

   because that would result in deadlock (it calls a series of all-to-one calls
   where each rank thinks it's the root).

