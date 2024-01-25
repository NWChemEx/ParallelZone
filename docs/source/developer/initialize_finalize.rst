.. Copyright 2024 NWChemEx-Project
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

.. _understanding_runtime_initialization_finalization:

#################################################
Understanding Runtime Initialization/Finalization
#################################################

:ref:`mpi` requires users to call ``MPI_Init``  to start MPI and
``MPI_Finalize`` to end it. MPI requires that each of these functions be called
only once, regardless of how many code units actually use MPI, i.e., managing
the lifetime of resources such as MPI processes and adhering to :ref:`raii` can
be tricky. This page works through some scenarios to help the reader become
better acquainted with the complexities.

************
RAII and MPI
************

ParallelZone opts to manage MPI through RAII. To do this we associate the
lifetime of MPI with the lifetime of a ``Runtime`` object. When a ``Runtime``
object is created it is either initialized with an existing MPI communicator or
it initializes MPI and then uses the MPI communicator resulting from
initialization. Each ``Runtime`` object internally tracks whether it initialized
MPI or not. When a ``Runtime`` object is destructed it will only call
``MPI_Finalize`` if ``*this`` initialized MPI.

.. note::

   At present there is no user-accessible ``Runtime`` object, rather users
   interact with an implicit ``Runtime`` through ``RuntimeView`` objects. When
   all ``RuntimeView`` objects go out of scope the implicit ``Runtime`` object
   is destructed. This decision stems from not wanting accidental/implicit
   copies to inadvertently shut down MPI.

.. _traditional_solution:

********************
Traditional Solution
********************

Many existing libraries deal with the MPI problem in one of two ways:

1. Assume the user will manage MPI. Thus the library requires the user to
   provide an already initialized MPI communicator.
2. Define functions like ``initialize`` / ``finalize`` which wrap MPI's
   ``MPI_Init`` / ``MPI_Finalize`` functions respectively.

From the perspective of PZ Scenario 1 is the easiest to deal with because it
means PZ is free to manage the lifetime of MPI however it wants, so long as MPI
is finalized after the library is done with it. Scenario 1 works well with our
RAII solution "out of the box" and is not considered further.

Scenario 2 is much harder because we know the library's ``initialize`` and
``finalize`` functions will contain MPI functions. This is because they will
minimally contain ``MPI_Init`` and ``MPI_Finalize``, but the functions may also
check if MPI has been initialized and finalized (this is a common practice to
avoid accidentally calling ``MPI_Init``/``MPI_Finalize`` after MPI has already
been initialize/finalized). It is also conceivable that these functions do
additional initialization/finalization which requires MPI to be initialized, but
not yet finalized, e.g., calls to synchronize data.

.. _raii_interacting_with_traditional_solution:

******************************************
RAII Interacting With Traditional Solution
******************************************

In :ref:`traditional_solution` we noted that when a library provides its own
``initialize`` / ``finalize`` functions (which we called "Scenario 2") RAII
interactions become more complicated. It's worth noting that Scenario 2 has two
sub-scenarios:

a. User should only call ``initialize`` and ``finalize`` if the library is
   managing MPI.
b. The user should always call ``initialize`` and ``finalize``.

Each of these sub-scenarios can occur interact with ParallelZone in one of two
states: PZ started MPI or PZ did not start MPI. Sub-scenario a is essentially
the same as Scenario 1 in :ref:`traditional_solution` if ParallelZone starts
MPI. If, however, the library starts MPI we have:

.. code-block:: c++

   initialize(); // library starts MPI
   auto comm = get_mpi_communicator_from_library();
   RuntimeView rv(comm); // PZ uses MPI from library

   finalize(); // library releases MPI
   // end of code, rv is released

This is fine so long as destruction of ``rv`` is guaranteed not to use any
MPI functions (which we ultimately will not be able to guarantee, but we'll get
to that). For now we note that there is a better way to write this which will
work even if ``rv`` calls MPI functions, namely we force ``rv`` to go out of
scope before ``finalize`` is called:

.. code-block:: c++


   initialize(); // library starts MPI
   {
       auto comm = get_mpi_communicator_from_library();
       RuntimeView rv(comm); // PZ uses MPI from library
       // rv is released
   }

   finalize(); // library releases MPI
   // end of code

Moving on Scenario 2b, if the library starts MPI it is identical to when the
library starts MPI in Scenario 2a and no further comment is necessary. The
remaining condition is Scenario 2b with ParallelZone starting MPI:

.. code-block:: c++

   RuntimeView rv; // ParallelZone starts MPI
   auto comm = rv.mpi_comm();
   initialize(comm); // library uses MPI from PZ

   finalize();
   // end of code, PZ releases MPI

This is okay as long as ``rv`` is guaranteed to be in scope when ``finalize``
is called.

***********************
RAII Plus Encapsulation
***********************

:ref:`raii_interacting_with_traditional_solution` showed that our RAII solution
is fine as long as we control the order of destruction. This is a detail we'd
rather not leak to the user, especially if more initialization/finalization
functions are added later (or if some are removed). With the traditional
solution we can easily encapsulate this detail with something like:

.. code-block:: c++

   void initialize() {
       library_a::initialize();
       library_b::initialize();
   }

   void finalize() {
       library_b::finalize();
       library_a::finalize();
   }

   // User's code
   initialize(); // A initializes MPI, then B uses A's MPI

   finalize(); // B cleans up, then A finalizes MPI

As shown, the order of initialization/finalization is guaranteed by creating
wrappers around sub-library initialization/finalization. Users rely on the
wrappers and never need to worry about the order.

So now what about RAII? Let's start with Scenario 2b, and ParallelZone starting
MPI:

.. code-block:: c++

   RuntimeView initialize() {
       RuntimeView rv; // PZ starts MPI
       library_a::initialize(rv.mpi_comm());
       return rv; // Must keep rv alive
   }

   void finalize() {
        library_a::finalize();
   }

   // User's code
   auto rv = initialize();

   finalize(); // library finalizes
   // end of code, PZ ends MPI

While this works, it violates RAII because the user needs to remember to call
``finalize`` before the code ends or else there will be a resource leak. The
entire point of RAII is to avoid the possibility of leaks. If we want our
``RuntimeView`` to adhere to RAII we must find a way for the destructor of
``rv`` to call ``finalize`` before it stops MPI. The easiest way to do this is
with callbacks:

.. code-block:: c++

   RuntimeView initialize() {
       RuntimeView rv; // PZ starts MPI
       library_a::initialize(rv.mpi_comm());

       // Register that rv must call finalize upon destruction
       rv.stack_callback(library_a::finalize);
       return rv; // Must keep rv alive
   }

   // User's code
   auto rv = initialize();

   // end of code, PZ's dtor calls library_a::finalize() then ends MPI


*******
Summary
*******

- MPI leaks initialization/finalization concerns to all dependencies.
- This has led to many libraries leaking those same details to their
  dependencies too.
- When ParallelZone manages MPI we can use RAII to avoid leaking those details
  to our dependencies.
- RAII however requires that ``RuntimeView`` be able to hold callbacks.
