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
 *  @note BinaryViewBase is non-owning of the bytes it is a view of. It is the
 *        users responsibility to ensure the underlying memory remains valid.
 *
 *  @tparam IsConst Used to toggle whether this is implementing BinaryView or
 *                  ConstBinaryView respectively.
 */
template<bool IsConst>
class BinaryViewBase {
public:
    /// Type of each byte in this view
    using value_type = std::conditional_t<IsConst, const std::byte, std::byte>;

    /// Type of a pointer that can be used to (possibly) read/write to the
    /// the underlying bytes. The bytes are read-only if @p IsConst  is true.
    using pointer = value_type*;

    /// Type of a pointer to read-only bytes.
    using const_pointer = const value_type*;

    /// If @p IsConst is true/false, iterators of this type can't/can be used
    //  to modify the bytes the iterator points to.
    using iterator = pointer;

    /// Type of an iterator of read-only bytes
    using const_iterator = const_pointer;

    /// Type used for counting and offsets
    using size_type = std::size_t;

    /** @brief Creates an empty view.
     *
     *  The default object is a view of the nullptr and has a size of 0. It is
     *  indistinguishable from calling BinaryView(nullptr, 0).
     *
     *  @throw None No throw guarantee.
     */
    BinaryViewBase() noexcept : m_p_(nullptr), m_n_(0) {}

    /** @brief Creates a view of the bytes pointed to by @p p.
     *
     *  This ctor takes a pointer @p p to the beginning of a buffer. The buffer
     *  contains @p n elements of type @p T. The resulting BinaryViewBase
     *  instance will be a view of this buffer, but as bytes of raw memory.
     *
     *  @tparam T The type of the objects in the input buffer.
     *
     *  @param[in] p A pointer to the 0-th element in the buffer.
     *
     *  @param[in] n The number of elements in the buffer. This is in general
     *               not the number of bytes.
     *
     *  @throw std::runtime_error if @p p is the nullptr and @p n is not 0.
     *                            Strong throw guarantee.
     */
    template<typename T>
    BinaryViewBase(T* p, size_type n);

    /** @brief Returns an iterator to the first byte in the view.
     *
     *  @return The bytes the resulting iterator points to are read/write if
     *          @p IsConst is false and read-only if @p IsConst is true.
     *
     *  @throw None No throw guarantee.
     */
    iterator begin() noexcept { return data(); }

    /** @brief Returns an iterator to the first byte in the view.
     *
     *  @return An iterator which points to read-only bytes.
     *
     *  @throw None No throw guarantee.
     */
    const_iterator begin() const noexcept { return data(); }

    /** @brief Returns an iterator to just past the last byte in the view.
     *
     *  @return The bytes the resulting iterator points to are read/write if
     *          @p IsConst is false and read-only if @p IsConst is true.
     *
     *  @throw None No throw guarantee.
     */
    iterator end() noexcept { return data() + size(); }

    /** @brief Returns an iterator to just past the last byte in the view.
     *
     *  @return The bytes the resulting iterator points to are read-only.
     *
     *  @throw None No throw guarantee.
     */
    const_iterator end() const noexcept { return data() + size(); }

    /** @brief Returns a pointer to the first byte in the view.
     *
     *  This method will return a pointer to the first byte in the view. If
     *  the view is empty then the returned pointer will be the nullptr.
     *
     *  @return The bytes the resulting pointer points to are read/write if
     *          @p IsConst is false and read-only if @p IsConst is true.
     *
     *  @throw None No throw guarantee.
     */
    pointer data() noexcept { return size() > 0 ? m_p_ : nullptr; }

    /** @brief Returns a pointer to the first byte in the view.
     *
     *  This method will return a pointer to the first byte in the view. If
     *  the view is empty then the returned pointer will be the nullptr.
     *
     *  @return The bytes the resulting pointer points to are read-only.
     *
     *  @throw None No throw guarantee.
     */
    const_pointer data() const noexcept { return size() > 0 ? m_p_ : nullptr; }

    /** @brief The size of the binary buffer, in bytes.
     *
     *  BinaryViews offer a view of contiguous binary data. This method is
     *  used to determine how many bytes the contiguous binary data holds. This
     *  method always returns the number of bytes regardless of what type the
     *  objects actually are. For example the following assert should be true:
     *
     *  ```.cpp
     *     BinaryViewBase bb(double{1.1}, 1);
     *     assert(bb == 1 * sizeof(double));
     *  ```
     *
     *  @return The number of bytes *this is a view of.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept { return m_n_; }

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two BinaryViewBase instances are value equal if they both are views of
     *  the same number of bytes, and if those bytes are byte-by-byte value
     *  equal.
     *
     *  @param[in] rhs The instance we are comparing *this to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const BinaryViewBase& rhs) const noexcept;

    /** @brief Determines if *this is different from @p rhs
     *
     *  This method simply negates the result of comparing *this to @p rhs
     *  for value equality. See operator== for the definition of value
     *  equality.
     *
     *  @param[in] rhs The instance we are comparing to.
     *
     *  @return False if *this is value equal to @p rhs and true otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator!=(const BinaryViewBase& rhs) const noexcept;

private:
    /// A pointer to the first byte in the buffer
    pointer m_p_;

    /// The length (in bytes) of the buffer
    size_type m_n_;
};

} // namespace detail_

/// A view of read/write bytes
class BinaryView : public detail_::BinaryViewBase<false> {
public:
    // Pull in base class's ctors
    using detail_::BinaryViewBase<false>::BinaryViewBase;
};

/// A view of read-only bytes
class ConstBinaryView : public detail_::BinaryViewBase<true> {
public:
    // Pull in base class's ctors
    using detail_::BinaryViewBase<true>::BinaryViewBase;

    /** @brief Enables implicit conversion from BinaryView to ConstBinaryView
     *
     *  This ctor allows BinaryView instances to be implicitly converted to
     *  ConstBinaryView instances. This parallels the usual C++ practice of
     *  a type `T` being implicitly convertible to type `const T`.
     *
     *  @param[in] other The BinaryView we are viewing as read-only.
     *
     *  @throw None No throw guarantee.
     */
    ConstBinaryView(const BinaryView& other) :
      ConstBinaryView(other.data(), other.size()) {}
};

} // namespace parallelzone::mpi_helpers

#include "binary_view.ipp"