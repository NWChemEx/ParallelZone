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

.. _ram_class:

#############
The RAM Class
#############

As the name suggests, the ``RAM`` class provides an abstraction for the RAM a
process can access.

***********
Getting RAM
***********

``RuntimeView`` objects are containers of ``ResourceSet`` objects, which in
turn are containers of hardware including ``RAM``. Assuming we have a
``RuntimeView`` object ``rv`` we can get a ``RAM`` object by:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../tests/cxx/doc_snippets/ram.cpp
         :language: c++
         :lines: 29-33
         :dedent: 4

   .. tab:: Python

      .. literalinclude:: ../../../tests/python/doc_snippets/test_ram.py
         :language: c++
         :lines: 23-27
         :dedent: 8

The first call to ``ResourceSet::ram()`` retrieves a handle to the RAM which
is local to rank 0. The second call gets the ``RAM`` instance local to the
current process (which for rank 0 is the same as ``rank_0_ram``). Once you
have the ``RAM`` object you can see basic stats such as how much total memory
it has by using member functions, for example:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../tests/cxx/doc_snippets/ram.cpp
         :language: c++
         :lines: 35-37
         :dedent: 4

   .. tab:: Python

      .. literalinclude:: ../../../tests/python/doc_snippets/test_ram.py
         :language: c++
         :lines: 29-31
         :dedent: 8


*************************
All-to-One MPI Operations
*************************

MPI operations are typically categorized based on how many processes are sending
and how many are receiving. In "all-to-one" MPI operations every process
computes some data and sends it to a "root" process. For simplicity, this
tutorial will have process :math:`p` "compute" three integers: :math:`p`,
:math:`p+1`, and :math:`p+2`. If we want to collect all of these computed
results into rank 0's RAM we would do:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../tests/cxx/doc_snippets/ram.cpp
         :language: c++
         :lines: 39-43
         :dedent: 4

   .. tab:: Python

      .. note::

         MPI operations are presently limited to the C++ API. Consider using
         mpi4py for your Python-based MPI needs.

The resulting object ``all_data`` is a ``std::optional`` which has a value if
``rank_0_ram`` is local to the current process. We can exploit whether or not
``all_data`` has a value to avoid needing to remember which process was the
root for the gather operation. For example:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../tests/cxx/doc_snippets/ram.cpp
         :language: c++
         :lines: 45-49
         :dedent: 4

   .. tab:: Python

      .. note::

         MPI operations are presently limited to the C++ API. Consider using
         mpi4py for your Python-based MPI needs.
