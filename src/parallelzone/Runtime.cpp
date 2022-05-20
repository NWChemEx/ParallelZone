#include <parallelzone/runtime.hpp>

namespace parallelzone {

/// Determines if the default ctor with initialize madness
bool starts_madness_() {
  if (!madness::initialized) return true;
  return false;
}

/// Either initializes madness or returns the default world
madness::World& default_world_() {
  if (!madness::initialized) {
    int c = 1;
    char** v = nullptr; // As of MPI-2 you can actually pass null pointers
    return madness::initialize(c, v, SafeMPI::COMM_WORLD, false);
  }
  return madness::World::get_default();
}

Runtime::Runtime() :
  m_init_madness_(starts_madness_()),
  m_num_partitions_(1),
  m_world_(default_world_()) {}

Runtime::Runtime(int argc, char** argv) :
  m_init_madness_(true),
  m_num_partitions_(1),
  m_world_(madness::initialize(argc, argv, SafeMPI::COMM_WORLD, false)) {}

Runtime::Runtime(const MPI_Comm& comm) : Runtime(SafeMPI::Intracomm(comm)) {}

Runtime::Runtime(const SafeMPI::Intracomm& comm) :
  m_init_madness_(false),
  m_num_partitions_(1),
  m_world_(*madness::World::find_instance(comm)) {}

Runtime::~Runtime() {
    if(!m_init_madness_) return;
    madness::finalize();
}

} // namespace parallelzone
