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

using Hasher    = bphash::Hasher;
using HashValue = bphash::HashValue;
using HashType  = bphash::HashType;

inline auto make_hasher() { return Hasher(HashType::Hash128); }

/*! \brief Return a string representation of a hash
 *
 * The string in lower case hex representation
 * with the length of 2*number_of_bits/8 characters.
 *
 * \param [in] hash The hash
 * \return A string representing the hash
 */
inline std::string hash_to_string(const HashValue& hash) {
    return bphash::hash_to_string(hash);
}

/*! \brief Generates a hash string in a single call
 *
 * This can be used to easily obtain the hash string for several
 * objects at once without creating a Hasher object manually.
 *
 * \param [in] args Objects to hash
 * \return Hash string of the given object(s)
 */
template<typename... Args>
auto hash_objects(Args&&... args) {
    auto h = make_hasher();
    h(std::forward<Args>(args)...);
    return bphash::hash_to_string(h.finalize());
}

/*! \brief Generates a hash in a single call
 *
 * This can be used to easily obtain the hash of several objects at once
 * without creating a Hasher object manually.
 *
 * \param [in] args Objects to hash
 * \return 128 bit hash of the given object(s)
 */
template<typename... Args>
auto make_hash(Args&&... args) {
    auto h = make_hasher();
    h(std::forward<Args>(args)...);
    return h.finalize();
}

} // namespace pz

namespace std {
template<typename T>
void hash_object(const reference_wrapper<T>& ref, bphash::Hasher& h) {
    h(ref.get());
}

} // namespace std
