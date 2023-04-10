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

######################
Quick Reference / FAQs
######################

**********************************
How do I get the MPI communicator?
**********************************

Unless you are getting the communicator to give it to another library, please
don't use raw MPI communicators. ParallelZone's design is such that you should
be able to do every MPI operation directly through ParallelZone's API. If an
operation is missing (or performs poorly) please either make an issue, or
better yet, add the missing feature/fix the inefficiency.

To answer the question, given a ``RuntimeView`` object ``rv``:

.. code-block:: c++

   rv.mpi_comm();

********************************************
How do I get the current process's MPI rank?
********************************************

Like the MPI communicator, you in general shouldn't need the rank as interacting
with ``ResourceSet`` instances should suffice; however, if you still want it
then given a ``RuntimeView`` object ``rv``:

.. code-block:: c++

   rv.my_resource_set().mpi_rank();
