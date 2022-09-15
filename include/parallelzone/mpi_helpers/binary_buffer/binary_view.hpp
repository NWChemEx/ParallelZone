#pragma once
#include <cstddef>   // std::byte
#include <stdexcept> // std::runtime_error

namespace parallelzone::mpi_helpers {
namespace detail_ {

/** @brief Guts of BinaryView templated on  const vs. non-const.
 *
 *  Ultimately we need two BinaryView types a non-const and a const type. The
 *  implementations of the two BinaryView types really only differ in the
 *  member types. This means we really could implement these two types as a
 *  single templated class and use template meta-programming to correctly set
 *  the types; however, for a more user-friendly experience, we
 *  don't want BinaryView to be templated. Hence the the BinaryViewBase
 *  class, which is templated on the const-ness. BinaryView and ConstBinaryView
 *  are more or less strong types derived from the appropriate specialization
 *  of BinaryViewBase.
 *
 */
template<bool IsConst>
class BinaryViewBase {
public:
    using value_type = std::conditional_t<IsConst, const std::byte, std::byte>;

    using pointer = value_type*;

    using const_pointer = const value_type*;

    using iterator = pointer;

    using const_iterator = const_pointer;

    using size_type = std::size_t;

    BinaryViewBase() noexcept : m_p_(nullptr), m_n_(0) {}

    template<typename T>
    BinaryViewBase(T* p, size_type n);

    iterator begin() noexcept { return data(); }

    const_iterator begin() const noexcept { return data(); }

    iterator end() noexcept { return data() + size(); }

    const_iterator end() const noexcept { return data() + size(); }

    pointer data() noexcept { return size() > 0 ? m_p_ : nullptr; }

    const_pointer data() const noexcept { return size() > 0 ? m_p_ : nullptr; }

    /** @brief The size of the binary buffer, in bytes.
     *
     *  BinaryViews offer a view of contiguous binary data. This method is
     *  used to determine how many bytes the contiguous binary data holds.
     *
     *  @return The number of bytes *this is a view of.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept { return m_n_; }

    bool operator==(const BinaryViewBase& rhs) const noexcept;

    bool operator!=(const BinaryViewBase& rhs) const noexcept;

private:
    /// A pointer to the first byte in the buffer
    pointer m_p_;

    /// The length (in bytes) of the buffer
    size_type m_n_;
};

template<bool IsConst>
template<typename T>
BinaryViewBase<IsConst>::BinaryViewBase(T* p, size_type n) :
  m_p_(reinterpret_cast<pointer>(p)), m_n_(sizeof(std::decay_t<T>) * n) {
    if(p == nullptr && n > 0)
        throw std::runtime_error("Nullptr should have a size of 0. So you "
                                 "either meant to pass a non-null pointer, or "
                                 "you computed the size wrong...");
}

template<bool IsConst>
bool BinaryViewBase<IsConst>::operator==(
  const BinaryViewBase& rhs) const noexcept {
    if(size() != rhs.size()) return false;
    if(size() == 0) return true;
    return std::equal(begin(), end(), rhs.begin());
}

template<bool IsConst>
bool BinaryViewBase<IsConst>::operator!=(
  const BinaryViewBase& rhs) const noexcept {
    return !(*this == rhs);
}

} // namespace detail_

class BinaryView : public detail_::BinaryViewBase<false> {
public:
    using detail_::BinaryViewBase<false>::BinaryViewBase;
};

class ConstBinaryView : public detail_::BinaryViewBase<true> {
public:
    using detail_::BinaryViewBase<true>::BinaryViewBase;

    ConstBinaryView(const BinaryView& other) :
      ConstBinaryView(other.data(), other.size()) {}
};

} // namespace parallelzone::mpi_helpers
