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

.. _ram_design:

###################
Designing RAM Class
###################

As stated in :ref:`parallel_runtime_design` we need abstractions to represent
the physical hardware resources on the computer. The ``RAM`` class models
the random-access memory of a node.

*****************************
Why Do We Need the RAM Class?
*****************************

From the perspective of ParallelZone, the ``RAM`` class is primarily envisioned
as a means for users to specify the start/end points of one-to-all and
all-to-one MPI operations. End users can also use the ``RAM`` class to
figure out how much memory the process has access to, which can be useful for
selecting among algorithms (space-time trade-off).

*********************
Design Considerations
*********************

Including the considerations brought up in :ref:`parallel_runtime_design` and
:ref:`resource_set_design` we see the ``RAM`` class needing to consider:

1. Current "state"

   - Total capacity
   - Total free memory
   - Total used memory
   - Potentially the transfer rate

#. All-to-one and one-to-all MPI operations
#. Encapsulation of details
#. Automatic tracking of memory usage

*****************************
Architecture of the RAM Class
*****************************

.. _fig_ram:

.. figure:: assets/ram.png
   :align: center

   High-level architecture of the RAM class.

:numref:`fig_ram` illustrates the main pieces of the ``RAM`` class. These
pieces address the concerns by:

1. We use the PAPI runtime system for accessing information about the ``RAM``
#. A ``CommPP`` object is used for the MPI operations.
#. The use of the PIMPL idiom allows us to avoid leaking PAPI or the ``CommPP``
   object through the API.
#. This issue has not been resolved, but could be done through a custom
   allocator class tied to the ``RAM`` object.

*************
Proposed APIs
*************

Examples of all-to-one communication with ParallelZone.

.. code-block:: cpp

   auto rt = get_runtime();

   // Each resource set generates some data
   auto data = generate_data();

   // This gathers the data from all members of rt to resource set 0
   // Type U is deducible from type of data w/ template meta-programming
   std::optional<U> output = rt.at(0).ram().gather(data);

   if(output.has_value()){
       // This only gets run on rank 0
       // Do stuff with output
   }

   // Reductions are similar, but take an op to apply
   auto op = get_op_to_apply();

   // Similar to gather, this reduces the data down to rank 0's RAM
   // Type V is deducible from type of op w/ template meta-programming
   std::optional<V> output2 = rt.at(0).ram().reduce(data, op);

   if(output2.has_value()){
       // This only gets run on rank 0
       // Do stuff with output2
   }

Examples of one-to-all communications

.. code-block:: c++

   auto rt = get_runtime();

   auto data;
   if(me == rt.at(0)){
       data = fill_in_data();
   }

   // Value of data is ignored for ranks not equal to 0
   auto output = rt.at(0).ram().scatter(data);


Examples of one-to-one communications

.. code-block:: c++

   auto rt = get_runtime();

   std::optional<decltype(fill_in_data())> data;
   if(me == rt.at(0)){
       data.emplace(fill_in_data());
   }

   // Sends the data to rank 1, type U determined by template meta-programming
   std::optional<U> output = rt.at(1).ram().send(data, "some tag");

   if(output.has_value()){
       // This part is only run by rank 1
       // Do stuff with output
   }

The proposed APIs admittedly have some MIMD aspects to them in the
"if I'm root" logic. This could be hidden in a scheduler. Something like:

.. code-block:: c++

   auto future_2_data = rt.schedule([](){return fill_in_data();});

With the idea being the underlying scheduler chooses the root.

****************
Additional Notes
****************

The RAM class is presently a stub. In particular PAPI is not hooked up.
