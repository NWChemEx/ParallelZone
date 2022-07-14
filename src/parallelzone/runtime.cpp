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
  m_world_(start_madness(m_init_madness_, argc, argv, comm)) {
  
      m_progress_logger_ = comm.Get_rank() ?  
        std::make_unique<Logger>(make_null_logger()) :
	std::make_unique<Logger>(make_stdout_logger());

      m_debug_logger_ =
        std::make_unique<Logger>(make_stderr_logger());
}

Runtime::~Runtime() {
    if(!m_init_madness_) return;
    madness::finalize();
}


Logger& Runtime::progress_logger() {
    return *m_progress_logger_;
}

Logger& Runtime::debug_logger() {
    return *m_debug_logger_;
}

} // namespace parallelzone
