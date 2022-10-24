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

.. literalinclude:: ../../../tests/cxx/doc_snippets/runtime_view.cpp
   :language: c++
   :lines: 26-34

The first call to ``ResourceSet::ram()`` retrieves a handle to the RAM which
is local to rank 0. The second call gets the ``RAM`` instance local to the
current process (which for rank 0 is the same as ``rank_0_ram``).
