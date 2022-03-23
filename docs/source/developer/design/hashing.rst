.. _hashing_design:

##############
Hashing Design
##############

This page collects design notes regarding hashing. At the moment hashing is not
needed by ParallelZone and these notes are for posterity.

****************
What is Hashing?
****************

Hashing is the process of mapping an arbitrarily large data object to a
relatively small, fixed-size data object. The fixed-size data object is known as
a "hash value" or just "hash". The function that does the mapping is known as
the "hash function". In practice hashes are usually 256-bit (32 byte) integers
that are displayed as 64 digit hexadecimal integers.
