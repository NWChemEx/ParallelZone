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

.. _logger_view_design:

##############################
Designing the LoggerView Class
##############################

The discussion :ref:`logging_design` ends in concluding that ParallelZone needs
a ``LoggerView`` class. This page details the design and architecture of the
``LoggerView`` class and the underlying infrastructure needed to make it work.

*****************************
What is the LoggerView Class?
*****************************

The ``LoggerView`` class is the user-facing API of ParallelZone's logging
component.

************************************
Why do we need the LoggerView Class?
************************************

As summarized in :ref:`logging_design`, we are not aware of any existing logging
solutions that do everything we need. We have thus chosen to implement the
missing features, to our liking, within the ``LoggerView`` class. The 
``LoggerView`` class also provides a stable API for ParallelZone users should we
need to add additional logging capabilities or switch backends.

*************************
Existing Logger Solutions
*************************

:ref:`existing_logging_solutions` covered the existing logging solutions in
detail and will not be replicated here. Instead we point out that of the 
available choices, Spdlog has most of the features we want and is heavily 
supported. As seen in the next section we have opted to build our ``LoggerView``
infrastructure around Spdlog.

*****************
LoggerView Design
*****************

.. _logger_arch:

.. figure:: assets/logger_arch.png
   :align: center

   Architecture of the LoggerView class and its infrastructure.

For the first implementation of the logging component we adopted the simple
architecture shown in Figure :numref:`logger_arch`. Users of ParallelZone see
one class, ``LoggerView``. As a first pass, ``LoggerView`` simply provides APIs 
for printing arbitrary data at a specified severity level. Eventually, this may 
be expanded to support more fine-grained control over the log formatting.
Internally ``LoggerView`` is implemented by a ``LoggerPIMPL`` object. In this 
first pass the ``LoggerPIMPL`` is simply a wrapper around Spdlog and controlling 
the sink for the ``LoggerView`` class amounts to configuring the Spdlog object 
in the PIMPL. Ultimately we chose to use Spdlog because it is relatively 
lightweight, has enough features for our current needs, and is well supported.


LoggerPIMPL Design
==================

At the moment, the ``LoggerPIMPL`` class is primarily meant to isolate the
``LoggerView`` class from the details of its sink. Since most sinks are 
singletons (there's only one standard out, one standard error, one file with a 
specified path, etc.), we chose to delete the LoggerPIMPL class's copy and 
move ctor/assignment operators. Strictly speaking developers can get around this
by making a new PIMPL object. With more work (a factory pattern or singleton 
classes for each sink) the singleton nature of the sinks could be better 
enforced better. However, since ParallelZone only exposes very basic logging 
capabilities at the moment, there's no way for users to make new LoggerPIMPL
instances.

This design addresses the considerations remaining from  
:ref:`logging_design` by:

2. Multiple logging levels.

   - Gained by using Spdlog.

3. Concurrency aware.

   - Spdlog is thread-safe.

4. Different "sinks"

   - Provided by Spdlog.

5. Enable/disable logging

   - Spdlog supports log filtering by severity.
   - LoggerView can have a null pointer to represent null logging.

*********************
Future Considerations
*********************

The current design gets the job done, while being amenable to extension. In
particular we note:

- Can add ``Sink`` classes so users define their own sink behavior (Spdlog
  also allows custom sinks, so our ``Sink`` class would wrap theirs).