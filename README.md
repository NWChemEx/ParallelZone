<!--
  ~ Copyright 2022 NWChemEx-Project
  ~
  ~ Licensed under the Apache License, Version 2.0 (the "License");
  ~ you may not use this file except in compliance with the License.
  ~ You may obtain a copy of the License at
  ~
  ~ http://www.apache.org/licenses/LICENSE-2.0
  ~
  ~ Unless required by applicable law or agreed to in writing, software
  ~ distributed under the License is distributed on an "AS IS" BASIS,
  ~ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  ~ See the License for the specific language governing permissions and
  ~ limitations under the License.
-->

[![Actions](https://github.com/NWChemEx-Project/ParallelZone/workflows/C_C++_CI/badge.svg)](https://github.com/NWChemEx-Project/ParallelZone)

<!-- TODO: Uncomment when code coverage works again
[![Codecov](https://codecov.io/github/NWChemEx-Project/ParallelZone/branch/master/graphs/sunburst.svg?token=gdemefzIU7)](https://codecov.io/github/NWChemEx-Project/ParallelZone/branch/master)
-->

# ParallelZone

ParallelZone is a parallel
[runtime system](https://en.wikipedia.org/wiki/Runtime_system) designed to be
useful for object-oriented, task-based, scientific software. ParallelZone is
still far from a complete project, but the feature set is expected to grow
dynamically based on the needs of its consumers (primarily the NWChemEx
project).

A more detailed, user-centric, description of the project can be found
[here](https://nwchemex-project.github.io/ParallelZone/about.html).

# Features

- Object-oriented C++ bindings to MPI.
- Logging primitives.
- Primitives for hardware resources.

# Install

ParallelZone relies on CMake for configuring and building, meaning that
installation is usually achieved via a variation on:

```.cmake
git clone https://github.com/NWChemEx-Project/ParallelZone
cd ParallelZone
cmake -H. -Bbuild -D...
cmake --build build
cmake --build build --target install
```
More detailed install instructions can be found
[here](https://nwchemex-project.github.io/ParallelZone/install.html).

# Contributing

[Contributor Guidelines](https://github.com/NWChemEx-Project/.github/blob/1a883d64519f62da7c8ba2b28aabda7c6f196b2c/.github/CONTRIBUTING.md)
[Contributor License Agreement](https://github.com/NWChemEx-Project/.github/blob/master/.github/CONTRIBUTING.md#contributor-license-agreement-cla)
[Developer Documentation](https://nwchemex-project.github.io/ParallelZone/developer/index.html)
[Code of Conduct](https://github.com/NWChemEx-Project/.github/blob/master/.github/CODE_OF_CONDUCT.md)

# Acknowledgments

This research was supported by the Exascale Computing Project (17-SC-20-SC), a collaborative effort of the U.S. Department of Energy Office of Science and the National Nuclear Security Administration.
