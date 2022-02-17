********
Hashing
********

Hashing is basically mapping arbitrarily-sized data to generally shorter and
fixed-sized data, which are simply referred to as hashes. These hashes can be
used as indices of a data structure (a hash table) that can provide efficient
access and storage (average constant time complexity for search, insert, and
delete) for key-value pairs. The hash function that maps the original keys to
hashes (hash keys) is expected to be easy to compute and hard to collide, where
a collision means having identical hashes for different keys. Minimizing hash
collisions is a notoriously difficult problem as the number of keys grow, but a
very important one to avoid hard to debug problems. Hashing is used in different
applications including caches, databases, cryptography, and cybersecurity. In
NWChemEx, hashing is primarily used for the memoization and checkpoint/restart
infrastructure.

Memoization is a run-time optimization technique to speed up calculations by
storing the inputs and results of function calls (caching) and returning the
results when the same inputs occur again instead of recomputing the functions.
This can be implemented by using a hash table where the keys are the hashed
inputs of the functions and the values are the return values of these functions.
In NWChemEx, this hash table is implemented in the `Cache` class of PluginPlay. 

The required properties of a hash function for NWChemEx is listed below:

#. Minimize hash collisions. This is the most important property since hash
   collisions can give us wrong results and debugging such problems could be
   very difficult.
#. The hash function needs to account for the type of the objects. (See `issue
   #30 <https://github.com/NWChemEx-Project/ParallelZone/issues/30>`_)
#. The hashes should be reproducable and portable, i.e. they should not change
   for different runs, compilers, or platforms.
#. Any change in the input should result in a new hash, i.e. :math:`x` and
   :math:`x+\epsilon` should have different hashes.
#. Unlike cryptographic hashes, irereversibility of the hashes or statistical
   distribution of hashes is not important as long as the hashes do not collide.

There are various libraries that implements different hashing algorithms
developed for a variety of applications. For NWChemEx, we need a library that
provides a hashing function with the desired propoerties listed above along with
support for hashing custom classes, pointers, arrays, and STL containers.

#. `std::hash <https://en.cppreference.com/w/cpp/utility/hash>`_
#. `boost::hash <https://www.boost.org/doc/libs/1_62_0/doc/html/hash.html#hash.intro>`_
#. `BPHash <http://bennybp.github.io/BPHash/>`_
#. `Hash-library <https://github.com/stbrumme/hash-library>`_
