#pragma once
#include <memory>
#include <optional>
#include <parallelzone/serialization/binary.hpp>

namespace parallelzone::hardware {

namespace detail_ {

class RAMPIMPL;
}

/** @brief Provides a runtime API for interacting with memory
 *
 *  The RAM class is envisoned as being the primary vessel for tracking memory
 *  usage, and for facilitating getting/setting data from/to remote RAM objects
 *  (i.e., RAM-based one-to-one-, one-to-all, and all-to-one MPI calls).
 *
 *  With regards to tracking memory, Version 1.0 of the RAM class is only
 *  capable of telling the user how much RAM the current process has direct
 *  access to. Better memory tracking requires writing allocators which update
 *  the RAM instance when memory is freed (and having the downstream classes
 *  use the allocators).
 *
 *  TODO: MPI operations need to be templated and use template meta-programming
 *        to deduce return types. For now we just set all types to double and
 *        return std::optional<double>. The present implementations all throw
 *        std::runtime_error("NYI").
 */
class RAM {
public:
    /// Unsigned integral type used for offsets and counting
    using size_type = std::size_t;

    /// Type of the object implementing the RAM class
    using pimpl_type = detail_::RAMPIMPL;

    /// Type of the pointer holding the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    // -------------------------------------------------------------------------
    // -- Ctors, Assignment, Dtor
    // -------------------------------------------------------------------------

    /** @brief Creates a new RAM instance with no memory
     *
     *  @throw None No throw guarantee
     */
    RAM() noexcept;

    /** @brief Creates a new RAM instance with the provided state.
     *
     *  RAM instances are usually created by ResourceSet instances when the
     *  ResourceSet is being populated. The ResourceSet is capable of creating
     *  a RAMPIMPL and does so directly. Once the RAMPIMPL is created the RAM
     *  instance is initialized via this ctor.
     *
     *  @param[in] pimpl The state for the new RAM instance.
     *
     *  @throw None No throw guarantee.
     */
    explicit RAM(pimpl_pointer pimpl) noexcept;

    /** @brief Makes a deep copy of @p other
     *
     *  @param[in] other The RAM instance being deep copied.
     */
    RAM(const RAM& other);

    /** @brief Creates a new RAM instance by taking ownership of @p other 's
     *         state.
     *
     *  @param[in,out] other The instance whose state is being taken. After this
     *                       operation @p other is in a state consistent with
     *                       default initialization.
     *
     *  @throw None No throw guarantee.
     */
    RAM(RAM&& other) noexcept;

    /** @brief Overwrites the state in *this with a deep copy of @p rhs.
     *
     *  @param[in] rhs The object whose state is being copied.
     *
     *  @return The current instance, after overwriting its state with a deep
     *          copy of the state in @p rhs.
     *
     *  @throw std::bad_alloc if there's a problem copyin @p rhs's state. Strong
     *                        throw guarantee.
     */
    RAM& operator=(const RAM& rhs);

    /** @brief Overwrites the state in *this with the state in @p rhs.
     *
     *  @param[in,out] rhs The object whose state is being taken. After this
     *                     operation @p rhs is in a state consistent with
     *                     default initialization.
     *
     *  @return The current instance, after overwriting its state with the
     *          state in @p rhs.
     *
     *  @throw None No throw guarantee.
     */
    RAM& operator=(RAM&& rhs) noexcept;

    /// Default no throw dtor
    ~RAM() noexcept;

    // -------------------------------------------------------------------------
    // -- Getters
    // -------------------------------------------------------------------------

    /** @brief How much memory is managed by *this.
     *
     *  This method returns the total amount of memory managed by *this. The
     *  returned value does not account for memory currently in use. The
     *  exact definition of the returned value is XXX. TODO: specify once
     *  implemented, I'm assuming it'll either be what the vendor says it is or
     *  it'll be the amount of the memory the OS can actually use.
     *
     *
     *  @return The total amount of memory managed by *this. Existing
     *           allocations
     */
    size_type total_space() const noexcept;

    // -------------------------------------------------------------------------
    // -- MPI all-to-one operations
    // -------------------------------------------------------------------------

    /** @brief Given the type of the input, @p InputType, this will be the type
     *         of the result from calling gather.
     *
     *  @tparam InputType The type of the object being gathered.
     *
     */
    template<typename T>
    using gather_return_type = std::optional<std::vector<std::decay_t<T>>>;

    /** @brief Sends data from all members of the RuntimeView to the
     *         ResourceSet which owns *this.
     *
     *  @tparam InputType The type of the data being gathered.
     *
     *  @param[in] input The local data to send to the ResourceSet which owns
     *                   *this.
     *
     *  @return An std::optional containing the gathered data. Only the
     *          std::optional returned to the ResourceSet which owns *this has
     *          a value.
     */
    template<typename InputType>
    gather_return_type<InputType> gather(InputType&& input) const;

    template<typename InputType>
    gather_return_type<InputType> gather(const InputType* p, size_type n) const;

    /** @brief Given the type of the input, @p InputType, and the type of the
     *         reduction functor, @p FxnType, this will be the type of the
     *         result from calling reduce.
     *
     *  @tparam InputType The type of the object being reduced.
     *  @tparam FxnType Type type of the functor doing the reduction.
     *
     */
    template<typename InputType, typename FxnType>
    using reduce_return_type = std::optional<InputType>;

    /** @brief Collects data from all members of the RuntimeView and reduces it
     *         to the ResourceSet which owns *this.
     *
     *
     *  @tparam InputType The type of the data being reduced.
     *  @tparam FxnType   The type of the functor doing the reduction.
     *
     *  @return The result of the reduction. Only the ResourceSet which owns
     *          *this has a value. All other ResourceSet instances get back an
     *          empty `std::optional`
     */
    reduce_return_type<double, double> reduce(double input, double fxn) const;

    // -------------------------------------------------------------------------
    // -- Utility methods
    // -------------------------------------------------------------------------

    /** @brief Determines if *this has memory.
     *
     *
     *  @return True if *this has total_space() == 0 and false otherwise.
     *
     *  @throw None No throw guarantee
     */
    bool empty() const noexcept;

    /** @brief Exchanges the state in *this with that in @p other
     *
     *  @param[in,out] other The RAM object we are exchanging state with. After
     *                       this method @p other will contain the state which
     *                       was previously in *this.
     *
     *  @throw None No throw guarantee.
     */
    void swap(RAM& other) noexcept;

    /** @brief Determines if *this is value equal to @p rhs
     *
     *  Two RAM instances are value equal if they represent the same physical
     *  RAM. In practice this means that the RAM instances belong to the same
     *  ResourceSet and have the same total amount of space.
     *
     *  @param[in] rhs The RAM instance we are comparing to.
     *
     *  @return True if *this is value equal to @p rhs and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const RAM& rhs) const noexcept;

private:
    /// Code factorization for checking if the PIMPL is non-null
    bool has_pimpl_() const noexcept;

    /// Code factorization for asserting that the PIMPL is non-null
    void assert_pimpl_() const;

    using binary_data = serialization::binary_data;

    using binary_return_type = std::optional<binary_data>;

    binary_return_type gather_(const std::byte* data, size_type n) const;

    /// The object actually implementing *this
    pimpl_pointer m_pimpl_;
};

template<typename InputType>
RAM::gather_return_type<InputType> RAM::gather(InputType&& input) const {
    serializer s;
    s << std::forward<InputType>(input);
}

template<typename InputType>
RAM::gather_return_type<InputType> RAM::gather(const InputType* p,
                                               size_type n) const {
    const auto* as_bytes = reinterpret_cast<const std::byte*>(p);
    size_type n_bytes    = n * sizeof(InputType);

    auto serialized_data = gather_(as_bytes, n_bytes);

    gather_return_type<InputType> rv;

    // Early out for everybody, but the root
    if(!serialized_data.has_value()) return rv;

    auto& wrapped_value = serialized_data.value();

    // Wrapped_value contains (# of ranks)*n bytes
    auto n_ranks = wrapped_value.size() / n;
    typename decltype(rv)::value_type buffer(n_ranks);
    for(std::size_t i = 0; i < n_ranks; ++i) {
        const auto* b      = wrapped_value.data() + i * n;
        const auto* e      = wrapped_value.data() + (i + 1) * n;
        const auto* b_char = reinterpret_cast<const char*>(b);
        const auto* e_char = reinterpret_cast<const char*>(e);
        std::string x(b_char, e_char);
        buffer[i] = std::move(x);
    }
    rv.emplace(std::move(buffer));
    return rv;
}

/** @brief Determines if two RAM instances are different.
 *  @relates RAM
 *
 *  Two RAM objects are different if they are not value equal. See the
 *  documentation for RAM::operator== for the definition of value equal.
 *
 *  @param[in] lhs The RAM object on the left side of the operator
 *  @param[in] rhs The RAM object on the right side of the operator
 *
 *  @return False if @p lhs and @p rhs are value equal. True otherwise.
 *
 *  @throw None No throw guarantee.
 */
inline bool operator!=(const RAM& lhs, const RAM& rhs) { return !(lhs == rhs); }

} // namespace parallelzone::hardware
