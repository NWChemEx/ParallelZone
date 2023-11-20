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

##########################
The ParallelZone Worldview
##########################

.. note::

   TL;DR. In ParallelZone the ``RuntimeView`` instance is your handle to the
   runtime environment. ``RuntimeView`` is a container of ``ResourceSet``
   objects. ``ResourceSet`` objects describe the resources (*i.e.*, RAM, CPUs,
   GPUs, *etc.*) that processes have direct/local access to.

To use ParallelZone it is helpful to understand the abstraction model at a high
level.

*********************
Focusing on Resources
*********************

In ParallelZone we go beyond MPI by mapping tasks/data to hardware, bypassing
the rank concept to the extent possible. More specifically, in ParallelZone we
start by assuming that when a multi-process program starts running there is
some total set of resources (here a "resource" is a somewhat catchall
term that includes CPU, GPU, memory, etc.) available to that program. This
total set of resources may or may not be all of the resources on the computer
and it also may or may not be the case that ParallelZone can access all of the
program's resources. Regardless, the set of resources ParallelZone can access
forms the ``RuntimeView``.

.. _fig_nesting_runtime_views:

.. figure:: assets/nesting_runtime_views.png
   :align: center

   Here the blue oval depicts the total resources the program is allowed to use
   and the green oval shows the resources the program lets ParallelZone use. In
   the top scenario ParallelZone manages all of the program's resources, whereas
   in the bottom scenario it manages only some of the program's resources.

To better illustrate the relationship between the program's total resources
and the ``RuntimeView``, :numref:`fig_nesting_runtime_views` shows two possible
ways a ``RuntimeView`` may initialized. The top scenario shown in
:numref:`fig_nesting_runtime_views` occurs when the entire program uses
ParallelZone as its parallel runtime. In this scenario a job is spun up with
some set of resources and ParallelZone is allowed to manage all of them
(*N.B.*, while both scenarios in :numref:`fig_nesting_runtime_views`
show ParallelZone only managing one node ParallelZone can manage multiple
nodes). Another common scenario occurs when a program relying on ParallelZone is
being driven by another program. In this scenario, the program has access to
more resources than it shares with ParallelZone.

Programs built on ParallelZone treat ``RuntimeView`` as the full set of
resources regardless of whether it is or isn't.

*****************
Resource Affinity
*****************

Simply knowing the total amounts of resources available isn't quite enough. We
also need to know which resources the current process has an affinity for. In
ParallelZone we keep this simple by partitioning each process's resources into
two sets: those it has an affinity for and those it doesn't. The set of
resources a process has an affinity for is termed that process's
``ResourceSet``. The ``ResourceSet`` of a process is populated with the
resources in the ``RuntimeView`` which are located physically on the node where
the process is running.

.. _fig_resource_set_mapping:

.. figure:: assets/resourceset_mapping.png
   :align: center

   ParallelZone's view of the runtime environment for the common scenario of
   one process per node.

:numref:`fig_resource_set_mapping` illustrates how ParallelZone sees the
hardware in the runtime environment when the program has one process running on
each node. In this scenario ``RuntimeView`` is managing the entire runtime
environment, meaning the ``RuntimeView`` object can see both nodes. The
``RuntimeView`` is then split in to two ``ResourceSet`` instances, each instance
seeing the node the current process is running on. In this scenario the
``ResourceSet`` objects are disjoint, *i.e.*, they do not share resources.

ParallelZone does not restrict users to running one process per node. If a
user runs more processes per node, then ParallelZone will give each of those
processes its own ``ResourceSet``; however, the ``ResourceSet`` objects will no
longer be disjoint since each process on a node can see the same resources.


**********
The Vision
**********

At the moment ParallelZone's feature set is pretty bare bones. The medium term
goal is for the ``ResourceSet`` objects to have task schedulers. Users will
estimate the resources needed for a task and tell a particular ``ResourceSet``
to run the task. The schedulers will automatically figure out how to best run
the task based on runtime conditions. Longer term we want to take this a step
further and add task schedulers to ``RuntimeView``. The task schedulers on
``RuntimeView`` would accept task graphs and take care of assigning the tasks
in the graph to the schedulers in the individual ``ResourceSet`` objects.
