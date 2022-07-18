#pragma once

#include <madness/world/MADworld.h>
#include <parallelzone/runtime/resource_set.hpp>

namespace parallelzone::runtime {
namespace detail_ {
class RuntimeViewPIMPL;
}

/** @brief A view of the execution environment.
 *
 *  The runtime (environment) is where a program runs. The RuntimeView class
 *  provides an API for querying properties of the runtime. The "view" part of
 *  the name signifies that a RuntimeView instance does not own the state, but
 *  only aliases it.
 *
 *  Users should be aware that a RuntimeView instance may only be a slice of the
 *  actual runtime (either because other programs are running on the computer
 *  or because another part of the program is using resources).
 *
 *  In MPI terms think of RuntimeView as an intra-communicator paralleling
 *  MPI_COMM_WORLD; however, the underlying MPI_Comm need not be MPI_COMM_WORLD.
 *
 *  @note RuntimeView uses RAII (resource acquisition is initialization) to
 *        manage MADNESS/MPI. This means that if you do supply a handle to an
 *        already initialized MADNESS/MPI runtime, then RuntimeView will assume
 *        it is using MPI_COMM_WORLD and will initialize MADNESS/MPI if
 *        necesary. In the event that RuntimeView does initialize MADNESS/MPI,
 *        it is also responsible for tearing-down MADNESS/MPI when the last
 *        reference to MADNESS/MPI goes out of scope. Internally, this is
 *        managed with a shared_ptr. So if you want to ensure that MADNESS/MPI
 *        is not finalized while you're doing something, make sure you hold on
 *        to a RuntimeView.
 */
class RuntimeView {
public:
    /// Unsigned integral type used for counting, offsets, and indexing
    using size_type = std::size_t;

    /// Type Runtime uses for handles to MPI communicators
    using mpi_comm_type = MPI_Comm;

    /// Type of a read-only reference to a mpi_comm_type object
    using const_mpi_comm_reference = const mpi_comm_type&;

    /// Type of a MADNESS world (sort of it's MPI_Comm)
    using madness_world_type = madness::World;

    /// Type of a modifable reference to a MADNESS world
    using madness_world_reference = madness_world_type&;

    /// Type used to pass the program's argument count (i.e., "argc")
    using argc_type = int;

    /// Type used to pass the program's arguments (i.e., "argv")
    using argv_type = char**;

    using resource_set_type = ResourceSet;

    /// Type of the class managing the state of this class
    using pimpl_type = detail_::RuntimeViewPIMPL;

    /// Type of a pointer to the PIMPL
    using pimpl_pointer = std::shared_ptr<pimpl_type>;

    // -------------------------------------------------------------------------
    // -- Ctors, Assignment, Dtor
    // -------------------------------------------------------------------------

    /** @brief Automatically starts MADNESS (and MPI) if not started
     *
     *  This ctor calls the primary ctor passing in `argc = 0`,
     *  `argv = nullptr`, and `comm = MPI_COMM_WORLD`. See the primary ctor's
     *  description for more details.
     *
     */
    RuntimeView();

    /** @brief Initializes the runtime given the arguments to `main`
     *
     *  In C/C++ the `main` function takes two positional arguments, @p argc
     *  and @p argv. This ctor simply forwards @p argc and @p argv to the
     *  primary ctor (and sets `comm = MPI_COMM_WORLD`). See the primary ctor's
     *  description for more details.
     *
     *  @param[in] argc The number of arguments the program was called with.
     *                  In conventional usage, @p argc is >= 1, but a value of 0
     *                  is allowed if @p argv is a nullptr.
     *  @param[in] argv The argument to the program. In conventional usage
     *                  @p argv is minimally the name of the program. @p argv
     *                  may be a nullptr if @p argc is 0.
     */
    RuntimeView(argc_type argc, argv_type argv);

    /** @brief Creates a RuntimeView which aliases the provided MPI runtime.
     *
     *  This ctor is primarily meant for initializing a RuntimeView instance
     *  after MPI has already been started. This ctor dispatches to the primary
     *  ctor by passing `argc = 0` and `argv = nullptr`. See the primary ctor
     *  documentation for more details.
     *
     *  @param[in] comm The MPI_Comm this RuntimeView should alias.
     */
    explicit RuntimeView(const_mpi_comm_reference comm);

    /** @brief Creates a RuntimeView which aliases the provided MADNESS runtime.
     *
     *  This ctor is primarily meant for initializing a RuntimeView instance
     *  after MADNESS (and therefore MPI) have already been started. This ctor
     *  dispatches to the primary ctor by passing `argc = 0`, `argv = nullptr`,
     *  and whatever MPI_Comm corresponds to @p world. See the primary ctor
     *  documentation for more details.
     *
     *  @param[in] world The MADNESS runtime instance we are aliasing.
     *
     */
    explicit RuntimeView(madness_world_reference world);

    /** @brief Primary ctor for creating a new RuntimeView
     *
     *  Most of the other ctors dispatch to this ctor. The ctor first determines
     *  if MADNESS has been initialized. If MADNESS has not been initialized,
     *  MADNESS will be initialized and the resulting RuntimeView instance will
     *  be responsible for tearing down MADNESS when it is no longer in use
     *  (as determined by an internal counter). After ensuring MADNESS
     *  is initialized, this ctor will collect information about the computer
     *  and initialize the internal ResourceSets with said information.
     *
     *  @param[in] argc The number of arguments the program was called with.
     *                  In conventional usage, @p argc is >= 1, but a value of 0
     *                  is allowed if @p argv is a nullptr.
     *  @param[in] argv The argument to the program. In conventional usage
     *                  @p argv is minimally the name of the program. @p argv
     *                  may be a nullptr if @p argc is 0.
     *  @param[in] comm The MPI_Comm this RuntimeView should alias.
     *
     *  // TODO: Document throws
     */
    RuntimeView(argc_type argc, argv_type argv, const_mpi_comm_reference comm);

    RuntimeView(const RuntimeView&);
    RuntimeView& operator=(const RuntimeView&);
    RuntimeView(RuntimeView&&) noexcept;
    RuntimeView& operator=(RuntimeView&&) noexcept;

    /** @brief Releases the present RuntimeView instance
     *
     *  RuntimeViews behave somewhat like a shared_ptr. Copies of a RuntimeView
     *  increment an internal counter. As long as the inernal counter is
     *  non-zero the resources (i.e., MADNESS and MPI) will not be finalized.
     *  When the counter reaches zero (and assuming the original RuntimeView
     *  actually initialized MADNESS/MPI) MADNESS and MPI will be finalized.
     *
     *  @throw None No throw guarantee.
     */
    ~RuntimeView() noexcept;

    /// @return MPI Communicator, associated with MADWorld
    const_mpi_comm_reference mpi_comm() const;

    /// @return MADWorld, ref handle to madness::World
    madness_world_reference madness_world() const;

    /** @brief The number of resource sets in this instance.
     *
     *  @return The number of resource sets in this view.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept;

    /** @brief Used to determine if the current process is part of *this
     *
     *  If *this is wrapping MPI_COMM_WORLD than the current process is always
     *  part of *this. However, for other MPI intracomms this need not be the
     *  case. This method
     */
    bool has_me() const noexcept;

    /** @brief Swaps the state of *this with @p other.
     *
     *  This method simply swaps the pointers of the PIMPLs. As such all
     *  references, etc. to state in *this will remain valid, except that after
     *  this call the references now refer to state in @p other.
     *
     *  @param[in] other The instance to swap *this's state with.
     *
     *  @throw None No throw guarantee.
     */
    void swap(RuntimeView& other) noexcept;

private:
    /// Holds the actual state of the Runtime
    pimpl_pointer m_pimpl_;
};

} // namespace parallelzone::runtime
