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
install time, etc.). Second, there is the runtime system which is what the
coding language is built on (e.g., in C-based Python it's C, in C it's
assembly magic). Lastly, there is the runtime environment which includes things
managed by the operating system like environment variables, and hardware
devices.

When we say ParallelZone is a runtime, we really mean that it is a runtime
system. Since ParallelZone also provides interfaces for querying the runtime
environment we drop the distinction between runtime system and runtime
environment and just call ParallelZone a "runtime".

*************************
Why Do We Need a Runtime?
*************************

By its very nature, high-performance computing is focused on maximizing the
performance of software. Inevitably, this requires knowledge of the runtime
environment and the ability to effectively parallelize over the existing
hardware.

**************************************
What is the Scope of the ParallelZone?
**************************************

ParallelZone will ultimately be a C++ library used by most pieces of our
software stack.


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

Message Passing Interface (MPI)
===============================

For better or for worse MPI is widely used for process parallelism. If our
software is going to play well with others, the process parallelism aspect of
our runtime needs to be built on MPI.

Hardware Introspection
======================

Different machines have different numbers of cores, GPUs, etc. We need to be
able to query this information. Furthermore, new hardware will emerge and become
relevant. The hardware detection of the runtime will need to grow as the
list of available hardware grows. Thus the runtime must not limit itself to
a particular set of hardware. At this point in time we foresee our algorithms
needing knowledge (both globally and process local) of:

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
serialization) can be largely automated if reflection exists.

Parallelization
===============
