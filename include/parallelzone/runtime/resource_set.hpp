#pragma once
#include <memory>

namespace parallelzone::runtime {

namespace detail_ {
class ResourceSetPIMPL;
}

/** @brief A set of runtime resources.
 *
 *  We assume that when the program is started it has a finite amount of
 *  resources (CPUs, GPUs, memory, etc.) available to it. Furthermore, we
 *  assume that those resources are somehow partitioned into sets (in most, if
 *  not all, cases this is done by a job scheduler). The ResourceSet class
 *  is a runtime abstraction for interacting with one of these sets of
 *  resources.
 *
 *  In MPI terms, a ResourceSet maps to an MPI group. That is it is a set of
 *  resources and Workers (ranks, in MPI terms) which can see those resources.
 *  There may be some resources that every Worker can see (think a
 *  parallel filesystem), but in general each Worker can only see a subset of
 *  the resources in the ResourceSet (the ResourceSet class internally knows
 *  this mapping).
 *
 */
class ResourceSet {
public:
    /// Unsigned type used for offsets and indexing
    using size_type = std::size_t;

    /// TODO: Set to Worker when class exists
    using worker_type = int;

    /// TODO: Set to Storage type when class exists
    using storage_type = int;

    /// The type of the class implementing the ResourceSet
    using pimpl_type = detail_::ResourceSetPIMPL;

    /// The type of a pointer to the PIMPL
    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    /** @brief Creates an empty ResourceSet
     *
     *  The ResourceSet created by this ctor has no workers and
     *  no resources.
     *
     *  @throw None No throw guarantee.
     */
    ResourceSet() noexcept;

    ResourceSet(pimpl_pointer pimpl) noexcept;

    ResourceSet(const ResourceSet& other);

    ResourceSet(ResourceSet&& other) noexcept;

    ResourceSet& operator=(const ResourceSet& rhs);

    ResourceSet& operator=(ResourceSet&& rhs) noexcept;

    ~ResourceSet() noexcept;

    /** @brief The number of Workers in *this
     *
     *
     *
     *  @return The number of Workers in this ResourceSet
     *
     *  @throw None No throw guarantee.
     */
    size_type n_workers() const noexcept;

    /** @brief True if *this contains no Workers and no resources
     *
     *  @return True if this ResourceSet is empty (has no workers and no
     *          resources) and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool empty() const noexcept;

    void swap(ResourceSet& other) noexcept;

    /** @brief Determines if *this is value equal to @p rhs.
     *
     *  Two ResourceSets are value equal if they contain the same resources,
     *  the same number of workers, and the workers are mapped to the
     *  resources in the same manner.
     *
     *  @return True if *this is value equal to @p rhs, and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const ResourceSet& rhs) const noexcept;

    /** @brief Determines if *this is different from @p rhs
     *
     *  This function simply negates a value equality comparison. See the
     *  documentation for operator== for the definition of value equality.
     *
     *  @return False if *this is value equal to @p rhs, and true otherwise.
     */
    bool operator!=(const ResourceSet& rhs) const noexcept;

private:
    pimpl_pointer m_pimpl_;
};

} // namespace parallelzone::runtime
