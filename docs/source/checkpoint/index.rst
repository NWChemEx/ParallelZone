=======================
Checkpoint/Restart
=======================
Checkpoint/Restart (C/R) techniques have been used to improve application
resilience against system faults and other events that terminate computations.
Fault rates are expected to increase substantially with exascale computing as
the number of processing elements rises into the tens of millions while the
circuit elements shrink in size. With the companion trend toward lower I/O to
compute ratios, the overhead of C/R tasks and impact on overall code performance
is expected to increase for exascale systems as well. And while exascale systems
will include novel high performance memory technologies (e.g. burst buffers,
nonvolatile memory), strategies to manage hierarchical memory for C/R are not
well established. With these challenges in mind, it is essential that exascale
application developers consider C/R capabilities early in their software
designs. Here, we identify suitable C/R techniques for NWChemEx.

In general, the input and output datasets from individual computational
chemistry calculations are relatively small - on the order of MB to GB. In
almost all cases, the size of input and output data is eclipsed by the enormous
amount of temporary “intermediate data” produced at runtime, data that are
discarded when the computation is complete. Significant challenges persist in
the efficient storage, distribution, and access of intermediate quantum
chemistry data - particularly in parallel computing. The movement, storage, and
manipulation of intermediate data represents the bulk of computational cost
during a quantum chemistry computation. Therefore, intermediate data is the
highest priority target for C/R capabilities and is the focus of the present
work. Intermediate data in NWChemEx resides in a cache, which maps unique hashes
of function parameters to results. The cache was designed to facilitate
archiving/checkpointing, memoization, and memory management in NWChemEx.
Intermediate data is typically stored in memory, but the NWChemEx memory manager
may utilize disk when memory resources are depleted (not implemented yet). 

Two general checkpoint strategies can be used to store intermediate data in the
NWChemEx design. The first is a high-resilience disk-based checkpoint that
stores checkpoint data on a parallel file system. In the second, in-memory
checkpointing technique employs distribution of redundant data across process
pairs. 

Disk-based Checkpointing 
--------------------------
Storing a checkpoint on a parallel file system is highly resilient against
system faults, but it is also very expensive in terms of the time it takes to
buffer data and move it to disk, i.e., I/O overhead. For our initial approach,
disk checkpointing will be the exclusive technique used for all NWChemEx data
with the exception of distributed tensors. Non-tensor data is typically “small”,
on the order of MB, and relatively inexpensive (quick) to store on disk.
Conversely, it is expensive to checkpoint “large” distributed tensors to disk -
so distributed tensors will rely on frequent in-memory checkpointing
capabilities and less frequent disk checkpointing. (Distributed tensors will be
checkpointed to disk at user-defined time intervals, and/or upon completion of
subtasks in hierarchical quantum chemistry methods. For example, the T1 and T2
amplitudes computed during a CCSD(T) job may be stored to disk after the CCSD
portion is completed, while in-memory checkpointing is used during the CCSD
iterations.) The previous NWChem code (a different implementation than NWChemEx)
stores C/R data in a custom disk-resident database structure. The NWChem
database can store geometry and basis set objects, arrays, and names of external
files that contain additional data.\ :sup:`1` NWChemEx checkpointing modules
will be more flexible in terms of what data can be checkpointed, and will rely
on established scientific format standards to facilitate interoperability and
parallel performance. The disk-based C/R design was guided by a data format and
management model based on the Hierarchical Data Format, version 5 (HDF5).\
:sup:`2`  HDF5 was chosen for initial prototyping because its rich parallel I/O
capabilities, familiar MPI-like API, and native support of powerful data
compression filters. The HDF5 file format is self-describing and portable across
different system architectures.

There are two primary types of HDF5 objects: datasets and groups. Datasets are
multidimensional arrays of data with metadata, and groups bundle multiple
datasets/groups along with additional metadata. The dataset metadata consists of
an ASCII name, a data type that describes the contained data, a dataspace that
describes the dataset dimensionality, and a storage layout (e.g., contiguous,
chunked). The conversion of NWChemEx data into the HDF5 representation is
assisted via a modified version of the Cereal serialization library.\ :sup:`3`
The library includes native support for binary, XML, and JSON archives; HDF5 is
being added as a custom archive type by the NWChemEx team as part of the work
described here. Cereal provides the infrastructure to quickly add serialization
capabilities with support for polymorphism, inheritance, and smart pointers. The
support for XML and JSON archives is also useful for providing users with
alternative data representations of checkpointed data. 

The prototype NWChemEx code includes two pathways for storing data in HDF5
checkpoint files, depending on the relative complexity of the checkpoint data.
Single-element and sequence containers that store data of homogenous and
predefined HDF5 datatypes are stored directly as HDF5 datasets with cereal. The
predefined HDF5 datatypes have significant overlap with the predefined MPI
datatypes. They include (unsigned) char, (unsigned) short, (unsigned) int,
(unsigned) long, (unsigned) long long, float, double, long double, bool, and
std::string. Associative containers (set, multiset, map, multimap) that consist
of predefined HDF5 datatypes are stored as datasets that are paired in an HDF5
group. In this case, the HDF5 group can be thought of as a set of keys that map
values stored in an HDF5 dataset. Cereal is used for all object tracking, and
template metaprogramming was used to implement the mapping functions that
convert from C++ types to HDF5 data types, and define the HDF5 dataspace.  For
cases where data types cannot be mapped to a predefined HDF5 type or cannot be
trivially represented as a group, the data is converted to  an opaque binary
data type and stored in a single dataset. While cereal can still handle object
tracking for opaque data, the data cannot be interpreted by HDF5 for random
access or application of bit filters. 

In-memory Checkpointing
------------------------
Checkpoints that are flushed to the parallel file system (PFS) are resilient
against a wide range of system faults, facility outages, and user errors, but
also introduce the most significant I/O overhead. Since hardware failures are
usually localized to a few nodes, it is practical to employ more efficient
checkpointing methods that only protect against failure on isolated nodes. One
approach is to create a redundant copy of  data that is local to a processor
“A”, and distribute the data copy to a separate processor “B”. If processor A
fails, then processor B can redistribute the redundant copy among the available
computing resources. Care must be taken to pair processors such that failure
events are independent of one another. For example, paired processors should not
exist on the same node or share a network connection or power supply. Pairing
processors in an optimal way may require consideration of previous hardware
failure patterns, or global system configurations such as network topology. This
checkpointing approach was previously applied to the CCSD(T) method in the
NWChem code using global arrays.\ :sup:`4` The performance overhead incurred by adding
fault tolerance capabilities ranged from 8.7%-9.9% for the CCSD phase, and just
1.5% for the triples component. The incremental performance degradation observed
for recovery from a single fault was just 0.71%. With the existing NWChem
implementation in mind, this general data redundancy checkpointing technique
can be used with NWChemEx in combination with full PFS checkpoints. The initial
target for redundant data checkpointing is large, distributed tensors. The
reasoning for this is two-fold. First, non-tensor data in quantum chemistry is
typically small (MB), so full checkpoints to disk will not add significant I/O
overhead. Small data structures will be checkpointed by flushing the NWChemEx
memoization cache to disk at regular intervals. Second, the majority of
computational work in NWChemEx will consist of operations on distributed
tensors. The data contained in the distributed tensors may change frequently as
computations proceed,  and losing any tensor data to hardware failure may
require that the application is rolled back to recompute the entire tensor. This
stresses the importance of frequent tensor checkpointing to minimize the amount
of work lost to system faults, which is achieved more efficiently with in-memory
checkpointing compared to disk methods. 

The PluginPlay consists of independent functions called Modules which are managed at
runtime by the ModuleManager. Each Module returns some computed property or
intermediate result, such that all computational chemistry methods are
implemented as compositions of multiple Modules. NWChemEx functionality can be
extended by creating new Modules which conform to the defined Module API. The
actual execution and composition of Modules into methods is handled by the
ModuleManager. Therefore, our approach also allows developers to modify or
replace existing Modules without requiring intimate knowledge of external
Modules. In general, restarting an application requires a checkpoint of the
application’s memory contents (application state), and some means to resume
execution from the last checkpointed state. In NWChemEx, state is an aggregation
of the data computed within each Module invoked during the computation. Each
module interacts with a Cache which is responsible for managing the module’s
state. At a high level, the Cache is a table which maps hashes of module input
parameters to the corresponding data computed by the module. 

The Cache class was designed to facilitate memoization of Modules. Memoization
is a run-time optimization technique which is implemented by storing the return
value of functions in a hash table. Subsequent calls to the functions with
identical input parameters will simply retrieve the stored results. While
memoization is typically used as a performance optimization technique, its
purpose was extended in NWChemEx to support C/R. During the course of a
computation, the memoization hash table (or results map) stored in the Cache is
periodically checkpointed to stable storage. Users can restart NWChemEx
computations by loading checkpointed result maps into the Cache. The module call
tree for the computation is fully re-traced, but the module function bodies are
not actually executed.  Instead, each module will automatically retrieve the
pre-computed memoized results from the Cache until parity is reached with the
checkpointed state. Cache instances will often store results from different
modules with a variety of result types. Following the type erasure pattern,
module results are wrapped in a container which can store arbitrary
types (similar to std::any). The Cache result map is implemented as an
std::map<hash_type, std::shared_ptr<Any>>.

The actual saving and loading of module states is also delegated to the Cache
class, with all data operations following the serialization and storage schemes
defined by an Archive instance. The Archive is the interface to the Cereal
archive infrastructure which is used for all disk-native checkpoint files.
Archives are initialized by specifying one of the supported archive types as a
template parameter, and providing a name for the archive file (or optionally, an
output stream object). The Cache constructor requires an Archive which is held
as an alternative type of a std::variant member within the Cache instance. Users
can optionally pass a std::chrono::seconds parameter to specify a time interval
between automated saves of the Cache results map. In an actual NWChemEx
calculation, the key string hash_val would be generated by hashing input
parameters to the module. The call to Cache::insert function adds an entry to
the Cache result map, with the key being hash_val and the value being a
std::shared_ptr<SDEAny> referencing h2o_geom. The Cache::save function saves the
result map entry specified by the hash value parameter. If only the hash value
is passed to the save function, then the result map entry will be stored to the
Archive held within the Cache std::variant. Alternatively, one can provide an
additional parameter specifying any external Archive. This feature is useful for
users who want to store results from only specific modules to separate files,
outside the scope of checkpointing.

References
------------
#. Mike Folk, Albert Cheng, Kim Yates. “HDF5: A file format and I/O library for
   high performance computing applications.” Proceedings of Supercomputing 1999,
   November 1999.
#. Hubertus Van Dam, Abhinav Vishnu, Wibe A. De Jong. “Designing a Scalable
   Fault Tolerance Model for High Performance Computational Chemistry: A Case
   Study with Coupled Cluster Perturbative Triples.” Journal of Chemical Theory
   and Computation 7, January 2011, 66-75.
#. Leonardo Bautista-Gomez et. al. “FTI:high performance fault tolerance
   interface for hybrid systems.” High Performance Computing, Networking,
   Storage and Analysis (SC) 2011, November 2011.
#. Adam Moody, Greg Bronevetsky, Kathryn Mohror, Bronis R. de Supinski. “Design
   Modeling, and Evaluation of a Scalable Multi-level Checkpointing System.”
   Proceedings of Supercomputing 2010, November 2010.
