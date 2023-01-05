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

.. _resource_set_design:

##########################################
Designing ParallelZone's ResourceSet Class
##########################################

The need for a ``ResourceSet`` class grew out of :ref:`parallel_runtime_design`.
This page fleshes the details out more.

*************************************
Why Do We Need the ResourceSet Class?
*************************************

When viewing resources we need to know the total set of resources, the
responsibility of which falls to ``RuntimeView``, and also which of those
resources are local/directly accessible by the current process. As discussed
in :ref:`parallel_runtime_design`, the ``ResourceSet`` class is ParallelZone's
abstraction for delimiting which resources are part of the process's local
runtime environment.

****************************
ResourceSet Responsibilities
****************************

As enumerated in :ref:`parallel_runtime_design` the main responsibilities of
the ``ResourceSet`` is to:

1. Scheduling per-thread tasks.
#. Tracking hardware instances available to the local runtime environment.
#. Facilitating data movement among ``ResourceSet`` instances.
#. Track resource usage.
#. Allow logical partitioning of resources.

Additional considerations:

6. Separate API concerns from backend.

   - May need to use different runtime systems in the future.
   - May need to support different hardware.
   - Hopefully keep dependencies private.

************************
ResourceSet Architecture
************************

.. _fig_resource_set:

.. figure:: assets/resource_set.png
   :align: center

   Illustration of the ResourceSet class and its major members.

:numref:`fig_resource_set` shows the architecture of the ``ResourceSet`` class.
It addresses the responsibilities by:

1. Having a scheduler which is in charge of scheduling thread-based tasks.
#. Instances for each hardware resource actually present.

   - Actual instances can be implicit (made on the fly)
   - ResourceSet's constructor is responsible for finding resources.

#. ``CommPP`` object to send/receive data in object-oriented manner.

   - We decided to have the hardware instances control the process of
     actually sending/receiving data. For example
     ``rs.ram().gather()`` would gather data into the RAM of the
     ``ResourceSet`` object ``rs``, it's called on.
   - Decision facilitates controlling the end-point (RAM or memory on the GPU).
   - Exposes a multi-process operation below ``RuntimeView``, which is somewhat
     awkward.

6. Accomplished by using PIMPL design paradigm.

At present:

- Tracking of resource usage
- Logical partitioning

are not yet implemented or designed.


****************
Additional Notes
****************

We right now assume one RAM bank. Should we generalize this?
