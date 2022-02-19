****************
Serialization
****************

Serialization is the process of translating an object into a format suitable for
storing or transmitting mainly with the goal of eventually reconstructing
(deserialization) the identical object. Unlike Python or Java, C++ doesn't have
built-in support for serialization, but there are various libraries that enables
serialization for most of the types in the C++ standard library directly and
user-defined classes through manually added serialization/deserialization
functions.

#. Support for C++ standard library types.   
#. Support for inheritance and polymorphism.
#. Serialization into JSON format. JSON is a human-readable format used by many
   quantum chemistry programs. 
#. Serialization into a portable binary format. Binary format provides the most
   efficient archive format both in terms of memory and computational cost.
   However, the format is implementation dependent since there is no standard in
   the binary format. A portable binary format should take care of `endianess
   <https://en.wikipedia.org/wiki/Endianness>`_.
#. Support for serializing distributed objects. 
#. Permissive license and reasonable documentation.
 
.. table:: Serialization libraries for C++
   :widths: auto

   ====================================================================================  ===== ===== ===== ===== ===== ===== 
    Library                                                                              1     2     3     4     5     6    
   ====================================================================================  ===== ===== ===== ===== ===== ===== 
   `Cereal <https://uscilab.github.io/cereal/index.html>`_                                ✅    ✅     ✅     ✅    ❌    ✅
   `MADNESS <https://github.com/m-a-d-n-e-s-s/madness>`_                                  ❌    ❌     ✅     ✅    ✅    ✅
   `Boost <https://www.boost.org/doc/libs/1_76_0/libs/serialization/doc/index.html>`_     ✅    ✅     ✅     ❌    ❌    ✅
   `Cista <https://cista.rocks/#main>`_  \ :sup:`1`                                       ✅    ❔     ❌     ❌    ❌    ✅    
   `zpp::bits <https://github.com/eyalz800/zpp_bits>`_ \ :sup:`2`                         ✅    ❔     ❌     ❔    ❌    ✅
   ====================================================================================  ===== ===== ===== ===== ===== ===== 

\ :sup:`1` Based on C++17 `structured bindings feature
<https://playfulprogramming.blogspot.com/2016/12/serializing-structs-with-c17-structured.html>`_
that enables some sort of reflection. 
\ :sup:`2` Requires C++20 features.

Currently, serialization within NWChemEx is based on MADNESS archive and Cereal libraries.
Natively MADNESS supports binary and XML archives; however, a thin wrapper for
Cereal archives has been added to support Cereal archive types like JSON and
portable binary archives. TiledArray and Libint libraries also use MADNESS
for serialization and MADNESS supports serializing distributed objects such as
TiledArray tensors. However, MADNESS does not support serialization of smart pointers. 
