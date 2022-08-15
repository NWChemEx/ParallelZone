.. _mpi_api_design:

#############################
ParallelZone's MPI API Design
#############################

The examples on this page show the target syntax for basic MPI operations using
ParallelZone. There's a couple of design notes:

- These are parallelism "primitives". In practice these primitives are used to
  implement parallel classes. Users of the classes should rarely (ideally
  never) need to deal with these primitives directly.
  - As an example think about how TA tensors work
- We strive to make the MPI code look SIMD as much as possible
  - ``std::optional`` on returns makes it easier to figure out who has the
    data without knowing the distribution
  - Each rank passes in the same information (aside from the data)
- Template meta-programming can be used to distinguish between passing in single
  values (say a ``double``) and a container of ``double``s (say
  ``std::vector<double>``)
  - Use a single binary-based API to go from template function to PIMPL
- By default other types need to be serialized before sending
  - Serialization/deserialization can be expensive for communication
  - Creative PIMPLs (say ones that allocate state in an arena) can be used to
    dramatically speed up serialization (actually makes it trivial)

Examples of all-to-one communication with ParallelZone.

.. code-block:: .cpp

   auto rt = get_runtime();
   auto me = rt.my_resource_set();

   // Each resource set generates some data
   auto data = generate_data(me.mpi_rank());

   // This gathers the data from all members of the rt to rank 0
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

.. code-block:: .cpp

   auto rt = get_runtime();
   auto me = rt.my_resource_set();

   auto data;
   if(me == rt.at(0)){
       data = fill_in_data();
   }

   // Value of data is ignored for ranks not equal to 0
   auto output = rt.at(0).ram().scatter(data);


Examples of one-to-one communications

.. code-block:: .cpp

   auto rt = get_runtime();
   auto me = rt.get_resource_set();

   auto data;
   if(me == rt.at(0)){
       data = fill_in_data();
   }

   // Sends the data to rank 1, type U determined by template meta-programming
   std::optional<U> output = me.ram().send(data, rt.at(1));

   if(output.has_value()){
       // This part is only run by rank 1
       // Do stuff with output
   }

Examples of all-to-all communications


.. code-block:: .cpp

   auto rt = get_runtime();

   auto data = generate_data(rt.my_resource_set().mpi_rank());

   // This is an all gather
   auto output = rt.gather<RAM>(data);

   // This is an all reduce
   auto output2 = rt.reduce<RAM>(data, op);
