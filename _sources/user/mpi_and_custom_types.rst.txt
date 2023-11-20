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

###########################
Using MPI With Custom Types
###########################

.. note::

   If your custom type is serializable you can safely ignore this page and your
   custom type will work correctly with ParallelZone. However, ParallelZone may
   not perform as efficiently as possible. To learn how to use ParallelZone more
   efficiently with custom types read on.

For better or for worse MPI was designed for functional programming. This
means its support for object-oriented programming is lacking out of the box.
Arguably the best way for ParallelZone to support arbitrary objects is by doing
all MPI operations on binary data. For an arbitrary C++ object this can always
be done by serializing/deserializing the object; however, depending on the type
of the object, and the properties of that type, there may be more efficient ways
to convert the object to/from binary.

Since C++ lacks reflection, ParallelZone relies on several type traits to
determine the properties of a particular type. These type traits are already
defined for standard types (types built in to the C++ language and those in the
standard library) and for types defined by ParallelZone. For all other types
ParallelZone will fall back to the safety of serialization/deserialization
unless told otherwise. This page explains the various type traits used by
ParallelZone. Users of ParallelZone should register their custom types with
these traits to get the most out of ParallelZone.

***************
NeedsSerialized
***************

The ``NeedsSerialized`` template class is used to determine if a type ``T``
needs to be serialized in order to be used by ParallelZone. By default
``NeedsSerialized`` is true for all ``T``. To make ``NeedsSerialized`` false
for a particular type ``NeedsSerialized`` needs to be specialized for that
type. For a type ``T`` this is as simple as:

.. code-block::

   namespace parallelzone::mpi_helpers {

   template<>
   struct NeedsSerialized<T> : std::false_type {};

ParallelZone already defines specializations for:

- ``std::vector<T>`` (for trivially copyable ``T`` only)
- ``BinaryBuffer``
- ``BinaryView``
- ``ConstBinaryView``

.. note::

   There are additional STL classes which should be covered, but are presently
   not coded up yet. Feel free to make a PR adding them.


*************************************
Other Traits That Impact MPI Behavior
*************************************

ParallelZone uses several other type traits under the hood, but generally
speaking users can safely ignore them. We list them here only for very
advanced usage of ParallelZone.

MPIDataType
===========

This type trait is used by ParallelZone to map a C++ type to its corresponding
MPI data type enumeration. For example this trait will map ``char`` to
``MPI_CHAR``, ``double`` to ``MPI_DOUBLE``, etc. Since MPI's data types are
only defined for primitive data types we have already exhaustively registered
the primitive C++ data type with it's corresponding MPI data type.

In some cases, users may have a user-defined type which is actually an MPI data
type (or can be implicitly converted to one). One common scenario is when using
enums or strong types. In these cases, users should specialize the
``MPIDataType`` class for their type.

MPIOp
=====

This type trait maps C++ functors to the corresponding MPI operation. For
example this type trait will map ``std::plus`` to ``MPI_SUM`` and
``std::multiplies`` to ``MPI_PROD``. Since there is a small set of known MPI
operations we have taken the liberty of defining the instantiating the
``MPIOp`` template class for these operations and users will typically not need
to worry about such mappings.

In some cases users may have user-defined functors which map to MPI operations.
In those cases they may choose to specialize ``MPIOp`` for their functor.
