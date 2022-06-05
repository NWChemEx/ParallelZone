#include <parallelzone/runtime.hpp>

namespace parallelzone {

namespace {

// Basically a ternary statement dispatching on whether we need to initialize
// MADNESS or not
auto& start_madness(bool initialize, int argc, char** argv,
                    const SafeMPI::Intracomm& comm) {
    if(initialize) return madness::initialize(argc, argv, comm, false);
    return *madness::World::find_instance(comm);
}

} // namespace

Runtime::Runtime() : Runtime(1, nullptr) {}

Runtime::Runtime(int argc, char** argv) :
  Runtime(argc, argv, SafeMPI::COMM_WORLD) {}

Runtime::Runtime(const MPI_Comm& comm) :
  Runtime(1, nullptr, SafeMPI::Intracomm(comm)) {}

Runtime::Runtime(const SafeMPI::Intracomm& comm) : Runtime(1, nullptr, comm) {}

// N.B. This ctor requires m_init_madness_ to be declared before m_world_
Runtime::Runtime(int argc, char** argv, const SafeMPI::Intracomm& comm) :
  m_init_madness_(!madness::initialized()),
  m_num_partitions_(1),
  m_world_(start_madness(m_init_madness_, argc, argv, comm)) {}

Runtime::~Runtime() {
    if(!m_init_madness_) return;
    madness::finalize();
}

  void Runtime::partition() {
    //resource_sets.reserve(num_partitions);
  }
  
} // namespace parallelzone
