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
type.

TODO: Finish me!!!
