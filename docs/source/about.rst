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
What is ParallelZone?
#####################

ParallelZone is a high-level object-oriented C++ runtime system designed to
facilitate writing high-performance, object-oriented C++ software. Under the
hood ParallelZone relies on your favorite legacy runtime systems (such as
MPI).


The main features of ParallelZone include:

- Object-oriented API. C++ is object-oriented. Your runtime should be too.
- SIMD-like API. ParallelZone's API is designed to help you write parallel code
  that looks serial.
- Support for process parallelism.
- Backwards compatible with MPI.

Planned features:

- Thread support
- Better hardware detection/introspection
