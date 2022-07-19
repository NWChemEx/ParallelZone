#include "detail_/runtime_view_pimpl.hpp"

// N.B. AFAIK the only way a RuntimeView can have no PIMPL is if an exception is
//      thrown in the ctor, the user catches the exception, and uses the
//      RuntimeView anyways. We assume that users won't do this and thus do not
//      assert that the PIMPL is non-null.

namespace parallelzone::runtime {

namespace {

// Basically a ternary statement dispatching on whether we need to initialize
// MADNESS or not
auto start_madness(int argc, char** argv, const MPI_Comm& comm) {
    bool initialize = !madness::initialized();
    madness::World* pworld;
    if(initialize) {
        // MADNESS doesn't appear to check if the provided comm is
        // MPI_COMM_WORLD, so if you pass an MPI_COMM it assumes MPI was
        // initialized. This if/else statement avoids passing a comm if we were
        // given MPI_COMM_WORLD
        if(comm == MPI_COMM_WORLD)
            pworld = &madness::initialize(argc, argv, true);
        else
            pworld = &madness::initialize(argc, argv, comm, true);
    } else
        pworld = madness::World::find_instance(SafeMPI::Intracomm(comm));

    return std::make_shared<detail_::RuntimeViewPIMPL>(initialize, *pworld);
}

} // namespace

// -----------------------------------------------------------------------------
// -- Ctors, Assignment, Dtor
// -----------------------------------------------------------------------------

RuntimeView::RuntimeView() : RuntimeView(0, nullptr) {}

RuntimeView::RuntimeView(argc_type argc, argv_type argv) :
  RuntimeView(argc, argv, MPI_COMM_WORLD) {}

RuntimeView::RuntimeView(const_mpi_comm_reference comm) :
  RuntimeView(0, nullptr, comm) {}

RuntimeView::RuntimeView(madness_world_reference world) :
  RuntimeView(0, nullptr, world.mpi.comm().Get_mpi_comm()) {}

RuntimeView::RuntimeView(int argc, char** argv, const_mpi_comm_reference comm) :
  m_pimpl_(start_madness(argc, argv, comm)) {}

RuntimeView::RuntimeView(const RuntimeView& other) noexcept = default;

RuntimeView& RuntimeView::operator=(const RuntimeView& rhs) noexcept = default;

RuntimeView::~RuntimeView() noexcept = default;

// -----------------------------------------------------------------------------
// -- Getters
// -----------------------------------------------------------------------------

RuntimeView::const_mpi_comm_reference RuntimeView::mpi_comm() const {
    return m_pimpl_->m_world.mpi.comm().Get_mpi_comm();
}

RuntimeView::madness_world_reference RuntimeView::madness_world() const {
    return m_pimpl_->m_world;
}

RuntimeView::size_type RuntimeView::size() const noexcept {
    return m_pimpl_->m_resource_sets.size();
}

bool RuntimeView::did_i_start_madness() const noexcept {
    return m_pimpl_->m_did_i_start_madness;
}

RuntimeView::resource_set_reference RuntimeView::at(size_type i) {
    bounds_check_(i);
    return m_pimpl_->m_resource_sets[i];
}

RuntimeView::const_resource_set_reference RuntimeView::at(size_type i) const {
    bounds_check_(i);
    return m_pimpl_->m_resource_sets[i];
}

bool RuntimeView::has_me() const { throw std::runtime_error("NYI"); }

// -----------------------------------------------------------------------------
// -- Utility methods
// -----------------------------------------------------------------------------

void RuntimeView::swap(RuntimeView& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

bool RuntimeView::operator==(const RuntimeView& rhs) const {
    throw std::runtime_error("NYI");
}

// -----------------------------------------------------------------------------
// -- Private methods
// -----------------------------------------------------------------------------

void RuntimeView::bounds_check_(size_type i) const {
    if(i < size()) return;
    throw std::out_of_range(std::to_string(i) + " is not in the range [0, " +
                            std::to_string(size()) + ").");
}

} // namespace parallelzone::runtime
