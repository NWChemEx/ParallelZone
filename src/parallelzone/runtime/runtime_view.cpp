#include "detail_/runtime_view_pimpl.hpp"

// N.B. AFAIK the only way a RuntimeView can have no PIMPL is if an exception is
//      thrown in the ctor, the user catches the exception, and uses the
//      RuntimeView anyways. We assume that users won't do this and thus do not
//      assert that the PIMPL is non-null.

namespace parallelzone::runtime {

namespace {

// Basically a ternary statement dispatching on whether we need to initialize
// MADNESS or not
auto start_madness(int argc, char** argv, const SafeMPI::Intracomm& comm) {
    bool initialize = !madness::initialized();
    madness::World* pworld;
    if(initialize)
        pworld = &madness::initialize(argc, argv, comm, false);
    else
        pworld = madness::World::find_instance(comm);
    return std::make_shared<detail_::RuntimeViewPIMPL>(initialize, *pworld);
}

} // namespace

// -----------------------------------------------------------------------------
// -- Ctors, Assignment, Dtor
// -----------------------------------------------------------------------------

RuntimeView::RuntimeView() : RuntimeView(0, nullptr) {}

RuntimeView::RuntimeView(argc_type argc, argv_type argv) :
  RuntimeView(argc, argv, SafeMPI::COMM_WORLD.Get_mpi_comm()) {}

RuntimeView::RuntimeView(const_mpi_comm_reference comm) :
  RuntimeView(0, nullptr, comm) {}

RuntimeView::RuntimeView(madness_world_reference world) :
  RuntimeView(0, nullptr, world.mpi.comm().Get_mpi_comm()) {}

RuntimeView::RuntimeView(int argc, char** argv, const_mpi_comm_reference comm) :
  m_pimpl_(start_madness(argc, argv, SafeMPI::Intracomm(comm))) {}

RuntimeView::RuntimeView(const RuntimeView& other) = default;

RuntimeView& RuntimeView::operator=(const RuntimeView& rhs) = default;

RuntimeView::RuntimeView(RuntimeView&& other) noexcept = default;

RuntimeView& RuntimeView::operator=(RuntimeView&& rhs) noexcept = default;

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

// -----------------------------------------------------------------------------
// -- Utility methods
// -----------------------------------------------------------------------------

void RuntimeView::swap(RuntimeView& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

} // namespace parallelzone::runtime
