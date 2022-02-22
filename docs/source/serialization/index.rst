****************
Serialization
****************

Serialization is the process of converting a data structure or an object state
into a format suitable for storing or transmitting. This is mainly performed
with the goal of eventually reconstructing (deserialization) the identical
object. Unlike Python or Java, C++ does not have built-in support for
serialization, but there are various libraries that enables serialization for
most of the types in the C++ standard library directly and user-defined classes
through manually added serialization/deserialization functions. We listed below
the desired properties of a serialization library for NWChemEx. 

#. Support for C++ standard library types.   
#. Support for inheritance and polymorphism.
#. Serialization into JSON format. JSON is a human-readable format supported by
   different languages and already used by various quantum chemistry programs. 
#. Serialization into a portable binary format. Binary format provides the most
   efficient archive format both in terms of memory and computational cost.
   However, the format is implementation dependent since there is no standard in
   the binary format. A portable binary format should take care of `endianess
   <https://en.wikipedia.org/wiki/Endianness>`_ and enable us to checkpoint and
   restart on different platforms.
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
\ :sup:`2` Requires C++20 features. Enables serialization of function calls
through a remote procedure call interface.

Along with the libraries in the table above, we also considered `Cereal_fwd
<https://github.com/breiker/cereal_fwd>`_:sup:`1` and `HPS
<https://github.com/jl2922/hps>`_:sup:`2` serialization libraries. However,
these libraries are not actively supported anymore and do not have a good
documentation. 

Currently, serialization within NWChemEx is based on MADNESS archive and Cereal
libraries. Natively MADNESS supports binary and XML archives; however, a thin
wrapper for Cereal archives has been added to support Cereal archive types like
JSON and portable binary archives. TiledArray and Libint libraries (NWChemEx
dependencies) also use MADNESS for serialization. MADNESS also supports
serializing distributed objects such as TiledArray tensors. On the other hand,
MADNESS does not have a good coverage for the serialization of standard library
types, particularly for smart pointers. 

Cereal provides a detailed `documentation <https://uscilab.github.io/cereal/>`_
about their API, supported archive types, and how to write custom serialization
functions. One drawback of Cereal is the requirement of explicit instantiation
of templated serialization functions for classes with pointers to private
implementations (PIMPL).

References
------------
#. K. Grochowski, M. Breiter, and R. Nowak, "Serialization in Object-Oriented
   Programming Languages", in Introduction to Data Science and Machine Learning.
   London, United Kingdom: IntechOpen, 2019.
   https://www.intechopen.com/chapters/68840 doi: 10.5772/intechopen.86917 
#. J. Li, "HPS: A C++ 11 High Performance Serialization Library." arXiv preprint
   arXiv:1811.04556, 2018. https://arxiv.org/pdf/1811.04556v2.pdf