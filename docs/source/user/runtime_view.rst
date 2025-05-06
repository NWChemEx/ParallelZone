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

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../tests/cxx/doc_snippets/runtime_view.cpp
         :language: c++
         :lines: 29-30
         :dedent: 4

   .. tab:: Python

      .. literalinclude:: ../../../tests/python/doc_snippets/test_runtime_view.py
         :language: python
         :lines: 24-25
         :dedent: 8

How to use a ``ResourceSet`` is covered in the next tutorial,
:ref:`resource_set_class`.

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

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../tests/cxx/doc_snippets/runtime_view.cpp
         :language: c++
         :lines: 32-39
         :dedent: 4

   .. tab:: Python

      .. note::

         MPI operations are presently limited to the C++ API. Consider using
         mpi4py for your Python-based MPI needs.

The value of ``results`` will be a :math:`3n` element vector with the elements:
:math:`0, 1, 2, 1, 2, 3, 2, 3, 4, \ldots, n, n+1, n+2` when the program is run
with :math:`n` processes.

********************
Program-Wide Logging
********************

Logging (the fancy term for printing) in a program distributed across several
processes can be a bit tricky. ParallelZone exposes two logging mechanisms, one
for logging program-wide state and one for logging process-local state.

.. warning::

   Logging program-wide state to the process-local logger is always okay. It
   usually just results in logging redundant information. Logging process-local
   information to the program-wide logger will usually not cause an error, but
   it may cause deadlock if every process does not call the program-wide logger.

Generally speaking, the program-wide logger should be used for all logging
needs except: when one needs to log distributed data, or when a logging
statement will only be executed by a subset of processes. As an example, say
we want to log the result of the all gather we performed in the previous
example, we can do this by:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../tests/cxx/doc_snippets/runtime_view.cpp
         :language: c++
         :lines: 41-42
         :dedent: 4

   .. tab:: Python

      .. note::

         The streaming syntax of the Logger is presently a C++-only feature.
         See the next code snippet for how to log in Python.

This snippet shows the streaming API of the ``Logger`` class. This is a
convenient API for users familiar with ``std::cout`` (or more generally
``std::ostream``) style printing, but can only be used with "info"-level
log messages...which brings us to log levels.

Logging primarily differs from traditional printing in that each log statement
is assigned a severity/importance. The severity levels tell you how severe the
message is. In ParallelZone the severity choices are (and their suggested
meanings):

- trace    - Logging that is considered verbose, even for debugging
- debug    - Logging that is likely only needed when trying to debug
- info     - Base-level logs, log messages primarily showing progress
- warn     - Something looks weird, it's not necessarily wrong, but it's odd
- error    - Something went wrong, but the program can recover
- critical - Something went wrong, but recovery is not possible

Severity increases from "trace" to "critical" such that "trace" is the least
important and "critical" is the most severe log statements. This means that
all logging statements set as trace and below are enabled if the severity of
the logger is set to trace. If the logger is set to debug, all debug, info,
warn, error, and critical message logs are enabled, while trace message logs
are not. For completeness, info includes info, warn, error, critical, and
warn includes warn, error, critical, and error includes error and critical,
and critical only includes critical. 


In practice, the value of a computed result usually falls under debug or
trace. There's at least two ways to do that:

.. tabs::

   .. tab:: C++

      .. literalinclude:: ../../../tests/cxx/doc_snippets/runtime_view.cpp
         :language: c++
         :lines: 44-49
         :dedent: 4

   .. tab:: Python

      .. literalinclude:: ../../../tests/python/doc_snippets/test_runtime_view.py
         :language: python
         :lines: 27-34
         :dedent: 8

The first example illustrates the use of the ``trace`` method. Each severity
level has its own corresponding method. The second example shows how to use
the more general ``log`` method. This particular overload of the ``log`` method
allows you to specify (at runtime if you like) the severity of the message.

********************
Python Example
********************

.. tabs::

   .. tab:: Python

      .. code-block:: python

         import parallelzone as pz

         log = pz.runtime.RuntimeView().logger()
         severity = pz.Logger.severity
         severities = [severity.trace, severity.debug, severity.info, severity.warn, severity.error, severity.critical] 

         log.set_severity(severity.critical)

         for level in severities:
            log.log(level, "Hello")

         # OUTPUT:
         # [2025-05-06 11:29:54.722] [Rank 0] [critical] Hello

         log.set_severity(severity.trace)

         for level in severities:
            log.log(level, "Hello")

         # OUTPUT:
         # [2025-05-06 11:33:05.203] [Rank 0] [trace] Hello
         # [2025-05-06 11:33:05.203] [Rank 0] [debug] Hello
         # [2025-05-06 11:33:05.203] [Rank 0] [info] Hello
         # [2025-05-06 11:33:05.203] [Rank 0] [warning] Hello
         # [2025-05-06 11:33:05.203] [Rank 0] [error] Hello
         # [2025-05-06 11:33:05.203] [Rank 0] [critical] Hello

.. note::

   Notice that we did not discuss where the log message gets printed. This is
   called the "sink". By default the program-wide logger has a sink which
   prints to standard out. When developing code with ParallelZone you should
   always use the loggers provided to your code, and not mess with the sinks.
   This is because the sinks are set by the person running the program to their
   liking.
