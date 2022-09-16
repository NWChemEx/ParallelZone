#pragma once

/** @file binary_buffer.ipp
 *
 *  This file contains inline definitions for members of BinaryBuffer and
 *  free functions associated with BinaryBuffer. This file should not be
 *  included from any file other than binary_buffer.hpp
 */

namespace parallelzone::mpi_helpers {

// -----------------------------------------------------------------------------
// -- Inline BinaryBuffer Method Implementations
// -----------------------------------------------------------------------------

inline BinaryBuffer::BinaryBuffer(size_type n) :
  BinaryBuffer([&]() {
      using internal_buffer = std::vector<value_type>;
      using pimpl_type      = detail_::BinaryBufferPIMPL<internal_buffer>;
      internal_buffer buffer(n);
      return std::make_unique<pimpl_type>(std::move(buffer));
  }()) {}

inline bool BinaryBuffer::operator==(const BinaryBuffer& rhs) const noexcept {
    if(size() != rhs.size()) return false;
    if(size() == 0) return true;
    return std::equal(begin(), end(), rhs.begin());
}

// -----------------------------------------------------------------------------
// -- Inline Free Functions
// -----------------------------------------------------------------------------

template<typename T>
BinaryBuffer make_binary_buffer(T&& input) {
    using clean_type = std::decay_t<T>;
    if constexpr(needs_serialized_v<clean_type>) {
        using pimpl_type = detail_::BinaryBufferPIMPL<std::string>;
        // TODO: Put directly into a string
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive ar(ss);
            ar << std::forward<T>(input);
        }
        auto pimpl = std::make_unique<pimpl_type>(ss.str());
        return BinaryBuffer(std::move(pimpl));
    } else {
        using pimpl_type = detail_::BinaryBufferPIMPL<clean_type>;
        auto pimpl       = std::make_unique<pimpl_type>(std::forward<T>(input));
        return BinaryBuffer(std::move(pimpl));
    }
}

template<typename T>
T from_binary_buffer(const BinaryBuffer& view) {
    // I think we will always need to copy out of a view, so there's no
    // reason to allow cv qualifiers or references.
    static_assert(std::is_same_v<T, std::decay_t<T>>);

    if constexpr(needs_serialized_v<T>) {
        using size_type = BinaryBuffer::size_type;
        // TODO: use boost::iostreams to avoid copy into stringstream.
        std::stringstream ss;
        for(size_type i = 0; i < view.size(); ++i)
            ss << *reinterpret_cast<const char*>(view.data() + i);

        T rv;
        {
            cereal::BinaryInputArchive ar(ss);
            ar >> rv;
        }
        return rv;
    } else {
        // Since we didn't need to serialize the T object going in, it must be
        // the case that T is basically a contiguous array of some type U. We
        // assume that U is given by T::value_type and that T has a range ctor
        // which takes two iterators.
        using value_type = typename T::value_type;
        const auto* p    = reinterpret_cast<const value_type*>(view.data());
        auto n           = view.size() / sizeof(value_type);
        return T(p, p + n);
    }
}

} // namespace parallelzone::mpi_helpers
