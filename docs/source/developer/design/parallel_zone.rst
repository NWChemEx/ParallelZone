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

.. _parallel_zone_design:

######################
Designing ParallelZone
######################

Software runs on hardware. Hardware varies from computer to computer. And the
available resources (cores, GPUs, RAM, etc.) of a computer vary with time.
While these three statements may sound obvious, in order for high-performance
software to perform its best, it needs to be aware of what hardware is
available and what load that hardware is under. ParallelZone is a runtime
designed to bridge the gap between our software and the current state of the
computer it runs on. This page captures the original design process of
ParallelZone. Keep in mind, this page is a snapshot in time and served as the
basis for starting ParallelZone. We fully expect ParallelZone to evolve over
time and become more than what is here.

*****************
What's a Runtime?
*****************

Strictly speaking there are several different, but related, concepts which get
labeled "runtime". First, there is the runtime execution phase of a program,
i.e., when the program is literally running (as opposed to say compile time,
install time, etc.). Second, there is the runtime system which comprises the
APIs that your software will use to interact with the operating system and
hardware during the runtime phase. In many programming scenarios these APIs are
several layers removed from the actual OS and hardware, *i.e.*, you're rarely
directly using device intrinsics or calling OS-specific functions. Lastly, there
is the runtime environment which is the state of the computer during the runtime
phase. This includes things like environment variables and hardware devices.

For simplicity we will continue to refer to ParallelZone as a runtime, but we
really mean that it is a runtime system.

**********************************
Why Do We Need a Runtime (System)?
**********************************

By its very nature, high-performance computing (HPC) is focused on maximizing the
performance of software. Inevitably, this requires a tighter coupling between
the software and the specifics of the runtime environment. The runtime system
provided by the C++ standard library notably lacks:

- support for parallelism (C++11 started to correct this, but as of this
  writing even the most modern C++ standards do not cover GPUs or distributed
  parallelism),
- hardware introspection,
- serialization

Hence we will need a runtime system to provide minimally these features.

**************************************
What is the Scope of the ParallelZone?
**************************************

ParallelZone will ultimately be a C++ library used by most pieces of our
software stack. We also want ParallelZone to be useful to developers/projects
outside the NWChemEx organization, and in turn we want to limit bloat.

To that end we limit ParallelZone to low-level, very general C++ operations
focused on parallelism and supporting that parallelism. Notably this excludes
linear algebra.


*********************
Design Considerations
*********************

This section lists what considerations affected the design of ParallelZone.
Many of these considerations warrant entire discussions. Here we provide brief
descriptions of the design considerations and link to the full discussion.

In no particular order we want/need our runtime to support:

Object-oriented C++17
=====================

Our software stack is object-oriented and written in C++17. The runtime must
thus support object-oriented C++17 if we are to call it.

Parallelization
===============

The heart of ParallelZone is parallelization. This includes, but shouldn't be
limited to:

- multi-process parallelism,
- CPU-threaded parallelism, and
- GPU-thread parallelism

There are a number of parallelism models. While it would be great if
ParallelZone could support all models, we are particularly interested in
task-based parallelism because it tends to be:

- SIMD-like (which maps to existing hardware better),
- more widely supported, and
- easier to parallelize.

Message Passing Interface (MPI) Support
=======================================

For better or for worse MPI is widely used for process parallelism. If our
software is going to play well with other high-performance software, the
process parallelism aspect ofour runtime needs to support MPI.

Scheduling
==========

We'd like our runtime to be capable of scheduling tasks for us. This includes
spawning the tasks and load balancing them. While easy to express, scheduling
is arguably the hardest part of writing parallel code. Generic solutions
greatly simplify implementing performant software downstream.

Hardware Introspection
======================

Generic scheduling solutions will need a performance model for the task and
knowledge of the runtime environment. Notably the scheduler needs to know what
resources are available and their specs. Heterogeneous systems seem to be here
to stay, so the runtime should be able to adapt as new hardware types are added.
At this point in time we foresee our algorithms needing knowledge of (both the
entire program and to the current process):

Central Processing Unit (CPU)
-----------------------------

As a first pass, knowledge of how CPU cores are available suffices. More
fine-grained optimization is possible by knowing cache sizes, and frequencies.
However, this can be punted until an algorithm is ready for such optimizations.

Graphics Processing Unit (GPU)
------------------------------

Similar to the CPU we want to know how many GPUs there are and how many threads
each supports. At the moment we also need to know the GPU vendor so we can
make the correct device calls.

Random Access Memory (RAM)
--------------------------

Available amount suffices for a first pass.

Disk
----

Available memory. Whether it's a hard disk drive or a sold state drive could be
useful for assessing how costly reading/writing to it will be. Alternatively,
the read/write speed would be useful for deciding when to store results versus
just recomputing them.

Logging
=======

For post facto analysis of a package it is essential to log information about
how software behaves at runtime. The logger should support multiple severity
levels, conditional logging, and be parallel aware (*i.e.* be able to handle
multiple processes/threads logging). Automatic timestamps and color is a plus.

Hashing
=======

Full discussion :ref:`hashing_design`.

Comparing objects can be expensive. If we establish a one-to-one mapping
between each instance of an object and a number (called the hash and usually
expressed in base 16) we can quickly compare objects by comparing the
corresponding hashes. Hashing has a number of applications ranging from
verifying data integrity to constant time lookup.

Serialization
=============

Full discussion :ref:`serialization_design`.

Objects are great for encapsulation, but at the end of the day low-level
operations typically only support a handful of types. Serialization is essential
for going from an arbitrary object to more primitive types and vice versa.

Reflection
==========

Full discussion :ref:`reflection_design`.

At the moment, C++ lacks reflection (the ability to introspect and modify a
program). Many generic programming tasks (for example hashing and
serialization) can be largely automated if reflection exists. While reflection
would be nice, as discussed in :ref:`reflection_design` we have decided not to
pursue this avenue.

*****************
Existing Runtimes
*****************

Depending on the definition of runtime there are a lot of possible choices
out there. In this section, we limit ourselves to runtimes that support
distributed parallelism. As a disclaimer, the information here is primarily
gleaned from skimming documentation and code, it may not reflect the actual
state of the codes. If there is an egregious error please open a PR with a fix.
Runtimes are listed in alphabetical order.

HPX
===

URL: `<https://github.com/STEllAR-GROUP/hpx>`_

First implementation of the ParalleleX programming model, which is notably an
alternative parallel programming model to message passing. In this analogy, HPX
is to ParalleleX as OpenMPI, MVAPICH, etc. are to message passing. The actual
runtime borrows heavily from C++ threading API, but extends it to distributed
computing. Based on the provided examples, programs are written in a SIMD-like
fashion relying on task-based parallelism.

Pros:
- Cross-platform
- C++ and Boost Standards compliant
- Active development

Cons:
- No GPU support?
- No MPI support?
- No hardware introspection

Legion
======

URL: `<https://github.com/StanfordLegion/legion>`_

Legion is another task-based runtime. One of the more unique features of Legion
is the scheduler. From the examples, it seems that the scheduler is capable of
optimizing how the tasks are run based on the available hardware.

Pros:
- Active development
- MPI support
- GPU support

Cons:
- API is very verbose, even for simple use cases
- Documentation is written at an expert level and hard to follow

MADNESS
=======

URL: `<https://github.com/m-a-d-n-e-s-s/madness>`_

MADNESS is a somewhat monolithic project containing:

- a parallel runtime system,
- a mathematics suite focusing on using multi-resolution analysis to solve
  integral and differential equations, and
- quantum chemistry methods

The parallel runtime system is SIMD-like and relies on object- and/or task-
based parallel programming models. The runtime relies on futures for
asynchronous operations and provides task schedulers.

Pros:
- Under TiledArray already
- Includes schedulers
- Support for GPUs
- Support for MPI

Cons:
- Relatively poor documentation
- Very heavy dependency
- More-or-less a single developer
- No hardware introspection

PaRSEC
======

URL: `<https://github.com/ICLDisco/parsec>`_

PaRSEC provides architecture aware scheduling and management of micro-tasks.
PaRSEC accomplishes this by modeling the algorithm as a directed acyclic task
graph where the nodes are tasks and edges represent data dependencies. PaRSEC
assumes the user will write the high-performance serial tasks and the runtime
concerns itself with scheduling these tasks, taking into account available
hardware and its current loads.

Pros:
- Support for GPUs
- Support for MPI

Cons:
- Relatively poor documentation

UPC++
=====

URL: `<https://bitbucket.org/berkeleylab/upcxx/wiki/Home>`_

UPC++ is a partitioned global address space programming model designed to be
interoperable with MPI and most threading runtimes (including those for GPUs).
UPC++ is designed for an SPMD model of execution. The API relies heavily on
futures, puts, and gets (the put/get calls can be for data or functions).

Pros:
- Support for a number of parallel runtimes
- Active development

Cons:
- Relatively low-level (i.e., still need to build infrastructure)
- Documentation is somewhat dense and difficult to use
- No hardware introspection

*********************
ParallelZone Strategy
*********************

Ultimately we couldn't find any runtime library out there which does everything
we want. However, just about every piece of functionality we want can be found
in an existing library. Writing a runtime system is a lengthy endeavor and we
do not want to do it. Thus our strategy is to design the runtime system API we
want and under the hood hook up as many libraries as we need to make that API
work. Given that there are a number of competing parallel runtimes currently
under heavy development, we anticipate that the innards of ParallelZone may be
somewhat turbulent. However, since the APIs of ParallelZone are meant to be
stable, ParallelZone represents a hedge meant to insulate downstream repos from
this turbulence.

*************************
ParallelZone Architecture
*************************

ParallelZone has the following pieces:

- Parallel Runtime
- Hardware
- Logger
- Utilities

Parallel Runtime
================

This is the biggest piece of ParallelZone. It is envisioned as containing the
routines and infrastructure needed to support task-based parallelism with SIMD
APIs. Infrastructure wise it should be capable of scheduling (including load
balancing) tasks on a wide variety of hardware. Under the hood it can accomplish
this by dispatching to other runtimes if it wants. The important part from the
perspective of the remainder of the stack is that the APIs remain consistent
and that they support whatever we need.

Hardware
========

This piece is made up of classes representing hardware components (CPU, RAM,
GPU, etc.). Like the parallel runtime, the classes in this piece are responsible
for providing the remainder of the stack with consistent stable APIs. Under the
hood the classes can be implemented by calling other libraries or via system
calls.

Logger
======

To a certain extent the logger is just a special case of a hardware element
(typically it's either redirected to standard out or a file). However, since
logging plays such a crucial role in debugging, profiling, and monitoring
program behavior it makes sense to call this component out specifically.

Utilities
=========

This is basically a grab bag of functionality needed to support the other
pieces. The primary piece is serialization.
