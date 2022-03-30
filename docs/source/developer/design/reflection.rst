.. _reflection_design:

#################
Reflection Design
#################

This page describes the considerations that went into the reflection API
ParallelZone relies on.

*******************
What is Reflection?
*******************

In object-oriented languages one distinguishes between the object's type and the
object's state (i.e., class vs. instance of a class). While most coding
scenarios focus on inspecting and manipulating an object's state, there are
times (usually when writing metafunctions) when the developer wants to inspect
and manipulate the object's class. A language is said to contain reflection if
at runtime it is possible to inspect the object's class, and to manipulate it.
Admittedly, the manipulation part is usually limited in statically typed
languages like C++.

At the time of writing, C++ has no native support for reflection. That said,
there are proposals to add reflection to the C++ language in an upcoming
standard; however, it is likely that the earliest standard which could include
reflection would be the one after C++23 (C++26?). Point being, reflection as
part of the C++ language is a ways away.

**************************
Why Do We Need Reflection?
**************************

Strictly speaking we don't need reflection, but the lack of reflection leads to
a lot of boilerplate when adding meta-operations (e.g. printing, value
comparison, hashing, and serialization). These operations all involve some form
of "for each attribute, do...".  Since C++ doesn't include reflection, the usual
solution is to hard-code the list of attributes into each operation. This works,
but it makes switching say hashing or serialization libraries a pain.
With reflection, we can keep interactions with such libraries in a single place.

*************************
Reflection Considerations
*************************

- Need to reflect polymorphic classes

   - Solution must be aware that a given object may actually have been declared
     as a more derived class than how it presently is being viewed, e.g., the
     function may be given a reference/pointer to a base class of the object

- Need to reflect classes that use PIMPL

   - Only some of the state of a class is visible from the API exposed in the
     declaration.
   - Reflection of the PIMPL will still occur, but it will only be visible from
     source files.

**************************
Reflection Implementations
**************************

Below is a list of C++ libraries implementing reflection without outside tools
such as schemas or code generation.

- Boost::Describe

   - https://github.com/boostorg/describe
   - Members are registered with a macro
   - Inspections return a list of "traits" classes
   - The "traits" classes have members which are accessible at both compile and
     runtime.
   - BSLv1
   - 4.8K stars and 289 watchers (Boost super project)
   - 30 stars and 6 watchers (Describe alone)

- Run Time Type Reflection (RTTR)

   - https://github.com/rttrorg/rttr
   - More verbose registration process than Boost::Describe, but also more
     features
   - Registration happens at runtime, so can't be used at compile time
   - MIT License
   - Last commit August 2021
   - 2.1K stars and 89 watchers

- CAMP

   - https://github.com/tegesoft/camp
   - MIT License
   - Unmaintained
   - 81 stars and 17 watchers

- CAMP (fork)

   - https://github.com/fw4spl-org/camp
   - Recommended fork of CAMP
   - Abandoned
   - 4 stars and 5 watchers.

- Ponder

   -https://github.com/billyquith/ponder
   - Another fork of CAMP, but then massively overhauled
   - API is very similar to RTTR
   - MIT License
   - Likely abandoned (Last commit Dec 2020).
   - 519 stars and 31 watchers

.. note::

   Statistics and assessments were accurate as of March 2022. The author does
   not have any experience with any of these libraries and notes are based off
   quickly perusing documentation and source code.

*******************
Reflection Strategy
*******************

Of the available libraries only Boost Describe and RTTR are currently supported.
In our opinion Boost Describe has the upper hand since its reflection is
accessible at compile time and runtime. The disadvantage of Boost Describe is
that you can only loop over base classes of the current class, not derived
classes; however, it should be possible to overcome this by using a virtual
function and wrapping the reflection algorithm in an object. A serialization
example, to make this more clear:

.. code-block:: c++

   class Serializer{
   public:
       template<typename Type2Serialize>
       void serialize(Type2Serialize&& obj2serialize) {
           /*
            * reflection of Type2Serialize is used here to implement
            * serialization
            */
       }
   };

   class BaseClass {
   public:
       virtual void serialize(Serializer& s) { s.serialize(*this); }
   };

   class DerivedClass : public BaseClass {
   public:
       virtual void serialize(Serializer& s) { s.serializer(*this); }
   };

   DerivedClass foo;
   BaseClass bar;
   BaseClass* pfoo = &foo;

   Serializer s;
   foo.serialize(s);   // Calls DerivedClass::serialize
   pfoo->serialize(s); // Calls DerivedClass::serialize
   bar.serialize(s);   // Calls BaseClass::serialize

Basically the ``Serializer`` instance wraps the actual serialization process for
an arbitrary type. We then rely on the virtual override to pass the most derived
type of the instance to the ``Serializer``.
