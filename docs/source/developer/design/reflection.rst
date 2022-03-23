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
