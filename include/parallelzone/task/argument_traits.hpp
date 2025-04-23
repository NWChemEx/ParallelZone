#pragma once
#include <type_traits>

namespace parallelzone::task {

/** @brief Structure that works out cv-qualifiers and reference-ness of @p T.
 *
 *  @tparam The type being inspected.
 *
 *  This structure is predominantly intended for use in template contexts where
 *  the properties of a type are not known. In those contexts, it can be
 *  useful to know if a type @p T is a reference, a const reference etc.
 */
template<typename T>
struct ArgumentTraits {
    /// Type of @p T w/o reference and cv-qualifiers
    using value_type = std::decay_t<T>;

    /// Type of a mutable reference to an object of type @p value_type
    using reference = value_type&;

    /// Type of a read-only reference to an object of type @p value_type
    using const_reference = const value_type&;

    /// Type of an rvalue reference to an object of type @p value_type
    using rvalue_reference = value_type&&;

    /// Type of a pointer to an object of type @p value_type
    using pointer = value_type*;

    /// Type of a pointer to a read-only object of type @p value_type
    using const_pointer = const value_type*;

    /// Is @p T consistent with being passed by value?
    static constexpr bool is_value_v = std::is_same_v<value_type, T>;

    /// Is @p T consistent with being passed as a const value?
    static constexpr bool is_const_value_v =
      std::is_same_v<const value_type, T>;

    /// Is @p T consistent with being passed by reference?
    static constexpr bool is_reference_v = std::is_same_v<reference, T>;

    /// Is @p T consistent with being passed by const reference?
    static constexpr bool is_const_reference_v =
      std::is_same_v<const_reference, T>;

    /// Is @p T consistent with being passed by rvalue reference?
    static constexpr bool is_rvalue_reference_v =
      std::is_same_v<rvalue_reference, T>;
};

} // namespace parallelzone::task