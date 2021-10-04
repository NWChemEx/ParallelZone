#pragma once
/** @file hasher.hpp
 *
 * This header provides the hashing API.
 * Currently, it is based on BPHash library.
 *
 */
#include <bphash/Hasher.hpp>
#include <bphash/types/All.hpp>
#include <functional>

namespace pz {

using bphash::hash_to_string;
using bphash::Hasher;
using bphash::HashType;
using bphash::HashValue;
using bphash::make_hash;

inline auto make_hasher() { return Hasher(HashType::Hash128); }

/** @brief Generates a hash string in a single call
 *
 * This can be used to easily obtain the hash string for several
 * objects at once without creating a Hasher object manually.
 *
 * @tparam Args variadic template for types of input objects
 * @param[in] args Objects to hash
 * @returns Hash string of the given object(s)
 */
template<typename... Args>
auto hash_objects(Args&&... args) {
    auto h = make_hasher();
    h(std::forward<Args>(args)...);
    return bphash::hash_to_string(h.finalize());
}

} // namespace pz

namespace std {
// Free function to make std::reference_wrapper hashable
template<typename T>
void hash_object(const reference_wrapper<T>& ref, bphash::Hasher& h) {
    h(ref.get());
}

} // namespace std
