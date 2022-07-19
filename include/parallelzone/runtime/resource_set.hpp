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

    /// TODO: Set to WorkerView when class exists
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
     *  no resources.The resulting ResourceSet is more or less a place holder.
     *  Assigning to the default constructed instance is presently the only
     *  way to make it non-empty.
     *
     *  @throw None No throw guarantee.
     */
    ResourceSet() noexcept;

    /** @brief Creates a ResourceSet with the provided state
     *
     *  Initializing ResourceSets is an implementation detail of ParallelZone.
     *  The actual initialization is done in the source files where the
     *  declaration of the PIMPL is visible. The initialization thus works by
     *  creating a PIMPL and then building a ResourceSet around it. This is
     *  the ctor used to build said ResourceSet.
     *
     *  @param[in] pimpl The state for the newly created ResourceSet.
     *
     *  @throw None No throw guarantee.
     */
    ResourceSet(pimpl_pointer pimpl) noexcept;

    /** @brief Makes a new ResourceSet which is a deep copy of @p other
     *
     *  @param[in] other The ResourceSet we are copying.
     *
     *  @throw std::bad_alloc if there is a problem allocating the PIMPL. Strong
     *                        throw guarantee.
     */
    ResourceSet(const ResourceSet& other);

    /** @brief Creates a new ResourceSet by taking the state from @p other
     *
     *  This ctor creates a new ResourceSet by transferring the state in
     *  @p other to *this.
     *
     *  @param[in,out] other The ResourceSet we are transferring the state from.
     *                       After this operation @p other will be in a state
     *                       consistent with default initialization.
     *
     *  @throw None No throw guarantee
     */
    ResourceSet(ResourceSet&& other) noexcept;

    /** @brief Overrides the state of *this with a copy of the state in @p rhs
     *
     *  This method will release the state held in *this (freeing it and in
     *  turn invalidating all references to it) and replace it with a deep
     *  copy of the state in @p rhs.
     *
     *  @param[in] rhs The instance whose state is being copied.
     *
     *  @return *this with its state set to a copy of the state in @p rhs.
     *
     *  @throw std::bad_alloc if there is a problem allocating the copy. Strong
     *                        throw guarantee.
     */
    ResourceSet& operator=(const ResourceSet& rhs);

    /** @brief Transfers ownership of @p rhs's resources to *this
     *
     *  This method will override the state of *this (releasing it and
     *  invalidating all references to it) and then transfer ownership of
     *  @p rhs's state to *this.
     *
     *  @param[in,out] rhs The ResourceSet we are taking the resources from.
     *                     After this operation @p rhs will be in a state
     *                     consistent with default initialization.
     *
     *  @return *this, after assigning @p rhs's state to *this.
     *
     *  @throw None No throw guarantee.
     */
    ResourceSet& operator=(ResourceSet&& rhs) noexcept;

    /// Default no-throw dtor
    ~ResourceSet() noexcept;

    // -------------------------------------------------------------------------
    // -- Getters
    // -------------------------------------------------------------------------

    /** @brief The number of Workers in *this
     *
     *  ResourceSets are usually shared by multiple workers. This method is used
     *  to determine how many workers have access to this resource set.
     *
     *  In MPI terms this corresponds to the number of processes that can see
     *  this ResourceSet.
     *
     *  @return The number of Workers in this ResourceSet
     *
     *  @throw None No throw guarantee.
     */
    size_type n_workers() const noexcept;

    // TODO: Make noexcept
    /** @brief Determines if the current process is in *this
     *
     *  Each ResourceSet is associated with a set of processes. This method can
     *  be used to determine if the current process is assocaited with *this.
     *
     *  @return True if the current process is in *this and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool has_me() const;

    // TODO: Make no throw (return an empty set if not in the ResourceSet)
    /** @brief Returns a view of the ResourceSet, as seen by the current
     *         process.
     *
     *  While each process in a resource set can see all of the resources in the
     *  set, each process can not in general access all of those resources
     *  directly. This method returns a view of the ResourceSet which only
     *  includes the resources directly accessible to the current process.
     *
     *  @return A view of *this with only the resources directly accessible to
     *          the current process.
     *
     *  @throw None No throw guarantee.
     */
    worker_type me();

    // -------------------------------------------------------------------------
    // -- Utility methods
    // -------------------------------------------------------------------------

    /** @brief True if *this contains no Workers and no resources
     *
     *  @return True if this ResourceSet is empty (has no workers and no
     *          resources) and false otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool empty() const noexcept;

    /** @brief Exchanges the state of two ResourceSet objects
     *
     *  This method will put the state of *this in @p other and take the state
     *  of @p other and put it in *this.
     *
     *  @param[in,out] other The ResourceSet we are swapping state with. After
     *                       this call @p other will contain the state which
     *                       was previously in *this.
     *
     *  @throw None No throw guarantee.
     */
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
    /// The object actually implementing the ResourceSet
    pimpl_pointer m_pimpl_;
};

} // namespace parallelzone::runtime
