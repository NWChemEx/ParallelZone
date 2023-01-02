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

.. _resource_set_class:

#####################
The ResourceSet Class
#####################

While a multi-process program operates within a potentially massive runtime
environment, the reality is that each process only has direct access to the
resources which are physically local to that process. Using other resources
requires sending data and/or tasks to another process. The ``ResourceSet``
class provides an abstraction for interacting with the resources local to a
given process.

*********************
Getting a ResourceSet
*********************

Each ``RuntimeView`` functions as a container of ``ResourceSet`` objects. The
number of ``ResourceSet`` objects will be equal to the number of processes
being managed by the ``RuntimeView``. Assuming ``rv`` is a ``RuntimeView``
object then we can get the ``ResourceSet`` for process 0 via:


.. literalinclude:: ../../../tests/cxx/doc_snippets/resource_set.cpp
   :language: c++
   :lines: 29-30

It is important to understand that ``rank_0_rs`` is the set of resources local
to process 0. So trying to use the resources in ``rank_0_rs`` from any process
other than rank 0 will usually involve communication. To avoid communication
one should work with their local ``ResourceSet`` which can be obtained by:

.. literalinclude:: ../../../tests/cxx/doc_snippets/resource_set.cpp
   :language: c++
   :lines: 32-33

You can quickly figure out if a ``ResourceObject`` describes local or remote
resources by:

.. literalinclude:: ../../../tests/cxx/doc_snippets/resource_set.cpp
   :language: c++
   :lines: 35-36

******************
Accessing Hardware
******************

``ResourceSet`` objects are containers of resources. As an example, let's say
we wanted to access the local RAM or the RAM local to rank 0. Then one could do:

.. literalinclude:: ../../../tests/cxx/doc_snippets/resource_set.cpp
   :language: c++
   :lines: 38-42

Using the RAM is described :ref:`ram_class`. Similar methods exist for accessing
other hardware such as the CPUs and GPUs. It is worth noting that not all
computers have all types of hardware. While we expect that just about every
computer has RAM, we can verify this by:

.. literalinclude:: ../../../tests/cxx/doc_snippets/resource_set.cpp
   :language: c++
   :lines: 44-48

Again similar methods are available for determining if other hardware is present
or not. These methods can be a convenient mechanism for dispatching based on
whether certain hardware is available or not.

*********************
Process-Local Logging
*********************

Program-wide logging is done through the ``RuntimeView``, process-local logging
is done through the ``ResourceSet`` class. Process-local logging works nearly
identical to program-wide logging. For example, if we want each process to
log whether its value of ``is_local`` this is done by:

.. literalinclude:: ../../../tests/cxx/doc_snippets/resource_set.cpp
   :language: c++
   :lines: 50-51

Compared to program-wide logging the main difference is that with process-local
logging each process can (but doesn't necessarily) log to a different sink.
This makes it much easier to figure out what each process did when going back
over the logs.

.. warning::

   Care has been taken to ensure that replicated data can always be written 
   to the process-local logger. The reverse is not true. In particular,
   deadlock can occur if the program-wide logger is called from a block of
   source code that is not executed by all processes. Sinks in general
   try to avoid implementations which can deadlock, but it is not
   guaranteed.

Typically process local-logging uses file sinks, with one file per process.
Especially if there's more than one process per filesystem this can have notable
performance degradation. For this reason, the process local sinks are by default
directed to null sinks (they don't print). Generally speaking, log messages
for process-local logs should have a severity of ``trace`` or ``debug`` as
most users will only turn them on when debugging is needed.