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

##########
Quickstart
##########

The ParallelZone class ``RuntimeView`` is an abstraction of the current
runtime environment. The ``RuntimeView`` groups the process local resources
(memory, processors, loggers, etc.) into objects of type ``ResourceSet``. Users
typically interact with ``RuntimeView`` and the ``ResourceSet`` object for
the current process. As an example:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../tests/cxx/doc_snippets/quickstart.cpp
         :language: c++
         :lines: 26-36
         :dedent: 4

   .. tab:: Python

      .. literalinclude:: ../../tests/python/doc_snippets/test_quickstart.py
         :language: python
         :lines: 24-34
         :dedent: 8

The ``RuntimeView`` also allows you to access other ``ResourceSet`` instances.
Typically this is done to send or receive information. For example, say we
wanted to copy data from each resource set into the RAM of the 0-th
resource set:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../tests/cxx/doc_snippets/quickstart.cpp
         :language: c++
         :lines: 38-45
         :dedent: 4

   .. tab:: Python

       .. note::

          MPI operations are presently limited to the C++ API. Consider using
          mpi4py for your Python-based MPI needs.

If we instead wanted every resource set to have a copy of the result, we do a
``RuntimeView``-wide gather:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../tests/cxx/doc_snippets/quickstart.cpp
         :language: c++
         :lines: 47-48
         :dedent: 4

   .. tab:: Python

       .. note::

          MPI operations are presently limited to the C++ API. Consider using
          mpi4py for your Python-based MPI needs.

Another major feature of parallelzone is introspection of the runtime
environment. Say we wanted to know how much total RAM each resource set has:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../tests/cxx/doc_snippets/quickstart.cpp
         :language: c++
         :lines: 50-51
         :dedent: 4

   .. tab:: Python

      .. literalinclude:: ../../tests/python/doc_snippets/test_quickstart.py
         :language: python
         :lines: 36-37
         :dedent: 8

To summarize:

- System-wide runtime environment state lives in the ``RuntimeView``.
- Operations involving every resource set getting data go through
  ``RuntimeView``.
- Environment state local to a single resource set goes through ``ResourceSet``,
  which is accessed via ``RuntimeView::at()``.
- Operations involving every resource set sending/getting data to/from a single
  resource set happen in ``ResourceSet``.
- Use the logger to print.
