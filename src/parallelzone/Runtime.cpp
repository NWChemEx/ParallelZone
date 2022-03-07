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
  Runtime(1, nullptr) {} // As of MPI-2 you can actually pass null pointers

Runtime::Runtime(int argc, char** argv) :
  initialized(true),
  num_partitions(1),
  mad_world(madness::initialize(argc, argv, SafeMPI::COMM_WORLD, false)) {}

Runtime::Runtime(const MPI_Comm& comm) : Runtime(SafeMPI::Intracomm(comm)) {}

Runtime::Runtime(const SafeMPI::Intracomm& comm) :
  initialized(false),
  num_partitions(1),
  mad_world(*madness::World::find_instance(comm)) {}

Runtime::~Runtime() {
    if(!initialized) return;
    madness::finalize();
}

} // namespace parallelzone
