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

#####################
The RuntimeView Class
#####################

Programs operate within a runtime environment. In ParallelZone the
responsibility for modeling the runtime environment falls to the
``RuntimeView`` class. The entire program runs in a single runtime environment.
``RuntimeView`` objects can "see" the singular runtime environment and interact
with it.

One of the first tasks that software built on ParallelZone will do
is obtain a ``RuntimeView`` object (analogous to how MPI-based software starts
MPI). The program will then pass that ``RuntimeView`` object to each sub call
(similar to passing an MPI communicator around). So if you are contributing to
software which already uses ParallelZone you will want to grab the
``RuntimeView`` instance via whatever mechanism your software affords, and not
create a new instance. How to get the ``RuntimeView`` is software specific, so
for the purposes of this tutorial we ignore it and assume you already have a
``RuntimeView`` instance ``rv``.

***********
First Steps
***********

One of the most common ``RuntimeView`` operations is to get the ``ResourceSet``
(the set of resources like RAM, CPUs, GPUs, etc.) that the current process has
direct access to. This is done by:

.. literalinclude:: ../../../tests/cxx/doc_snippets/runtime_view.cpp
   :language: c++
   :lines: 29-30

How to use a ``ResourceSet`` is covered in the next tutorial,
:ref:`resource_set_class`.

.. todo::

   Show how to get/use the logger.

******************************
All-to-All MPI-Like Operations
******************************

``RuntimeView`` objects have access to the entire runtime environment, thus
one can use ``RuntimeView`` objects to synchronize state across the entire
runtime environment. For example, say we wanted to do an all gather operation.
In an all gather operation each process computes some data, and gives that data
to every other process. As a trivial example we first show how to have MPI rank
:math:`r` "compute" three pieces of data, the integers :math:`r`, :math:`r+1`,
and :math:`r+2`. After computing the data, we synchronize it so that each
process has a copy of all the data.

.. literalinclude:: ../../../tests/cxx/doc_snippets/runtime_view.cpp
   :language: c++
   :lines: 32-39

The value of ``results`` will be a :math:`3n` element vector with the elements:
:math:`0, 1, 2, 1, 2, 3, 2, 3, 4, \ldots, n, n+1, n+2` when the program is run
with :math:`n` processes.
