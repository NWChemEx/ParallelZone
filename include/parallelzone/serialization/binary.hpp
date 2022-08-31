#pragma once
#include <cstddef>
#include <vector>

namespace parallelzone::serialization {

/** @brief Type of object holding (contiguous) raw bytes.
 *
 *  C++17 introduced the std::byte type for referring to a raw byte. In practice
 *  most objects are larger than a single byte and we need an object capable
 *  of holding multiple bytes. This typedef aliases an object which behaves like
 *  a contiguous array of std::byte instances. For API compatability purposes
 *  users and developers of ParallelZone are encouraged to use the
 *  binary_data typedef instead of directly using the underlying type.
 *
 */
using binary_data = std::vector<std::byte>;

} // namespace parallelzone::serialization
