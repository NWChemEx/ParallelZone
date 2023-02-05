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

############
ParallelZone
############

ParallelZone is a high-level, object-oriented C++ runtime system designed to
facilitate writing high-performance, object-oriented C++ software. ParallelZone
is conceived as being a stable :ref:`API` more than an implementation of a full
blown runtime system. By this we mean that under the hood ParallelZone simply
dispatches to more established runtime systems (such as :ref:`MPI`) and lets
them do the heavy lifting. Unfortunately, the space of runtime systems is highly
turbulent at the moment, which is why we designed ParallelZone to provide a
buffer that insulates downstream dependencies from the chaotic underpinnings.

The main features of ParallelZone include:

- Object-oriented API. C++ is object-oriented. Your runtime should be too.
- :ref:`SIMD`-like API. ParallelZone's API is designed to help you write
  parallel code that looks serial because that's the easiest kind to write.
- Support for process parallelism.
- Backwards compatible with MPI.

Planned features:

- Thread support
- Better hardware detection/introspection
- Asynchronous execution
- Hardware aware scheduling

.. toctree::
   :maxdepth: 1
   :caption: Contents:

   install
   abbreviations
   quickstart
   about
   user/index
   developer/index

`C++ API <parallelzone_cxx_api/html/index.html>`_
