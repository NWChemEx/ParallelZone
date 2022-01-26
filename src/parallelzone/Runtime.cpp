#include <parallelzone/Runtime.hpp>

namespace parallelzone {

using namespace madness;

namespace detail {
struct default_world {
    static World& get() {
        if(!world()) {
            assert((madness::initialized()) &&
                   "parallelzone::detail::default_world::get() called "
                   "before madness::initialize()");
            world() = &madness::World::get_default();
        }
        return *world();
    }
    static void set(World* w) { world() = w; }
    /// @return pointer to the default world, if set, or nullptr otherwise
    static World* query() { return world(); }

private:
    static World*& world() {
        static World* world_ = nullptr;
        return world_;
    }
};
} // namespace detail

Runtime::Runtime() :
  madness_world(detail::default_world::get()),
  initialized(true),
  num_partitions(1) {}

Runtime::Runtime(int& argc, char**& argv) :
  Runtime(argc, argv, SafeMPI::COMM_WORLD) {}

Runtime::Runtime(int& argc, char**& argv, const MPI_Comm& comm) :
  Runtime(argc, argv, SafeMPI::Intracomm(comm)) {}

Runtime::Runtime(int& argc, char**& argv, const SafeMPI::Intracomm& comm) :
  madness_world(isInitialized() ? madness::initialize(argc, argv, comm, false) :
                                  *madness::World::find_instance(comm)) {
    initialized    = true;
    num_partitions = 1;
}

Runtime::~Runtime() {
    initialized = false;
    madness::finalize();
}

} // namespace parallelzone
