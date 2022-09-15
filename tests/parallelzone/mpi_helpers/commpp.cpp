#include "../test_parallelzone.hpp"
#include <parallelzone/mpi_helpers/commpp.hpp>

using namespace parallelzone::mpi_helpers;

namespace {

// In these tests we declare an object "data: which holds some data to
// gather from (it's conveniently also the answer).
template<typename T>
void gather_buffer_kernel(std::size_t chunk_size, std::size_t root,
                          CommPP& comm) {
    using reference       = CommPP::binary_reference;
    using const_reference = CommPP::const_binary_reference;
    using size_type       = std::size_t;

    size_type max_elems    = comm.size() * chunk_size;
    size_type begin_offset = comm.me() * chunk_size;
    std::vector<T> data;
    std::vector<T> out_buffer(max_elems);
    for(std::size_t i = 0; i < max_elems; ++i) data.push_back(T{i});

    auto p = out_buffer.data();

    const_reference data_in(data.data() + begin_offset, chunk_size);
    reference buffer_in(out_buffer.data(), out_buffer.size());
    comm.gather(data_in, buffer_in, 0);

    if(comm.me() == root) {
        // We need to check that the root process has the correct value
        // and that gather didn't muck with the memory allocation of
        // out_buffer

        auto out_begin = out_buffer.begin();
        REQUIRE(std::equal(data.begin(), data.end(), out_begin));
        REQUIRE(p == out_buffer.data());
    } else {
        // out_buffer should be unchanged
        std::vector<T> corr(max_elems);
        REQUIRE(std::equal(out_buffer.begin(), out_buffer.end(), corr.begin()));
    }
}

} // namespace

TEST_CASE("CommPP") {
    using value_type      = CommPP::binary_type;
    using const_reference = CommPP::const_binary_reference;

    auto& world = testing::PZEnvironment::comm_world();

    CommPP comm(world.mpi_comm());
    auto n_ranks = comm.size();
    auto me      = comm.me();

    for(std::size_t root = 0; root < std::min(n_ranks, 3); ++root) {
        auto root_str = " root = " + std::to_string(root);
        for(std::size_t chunk_size = 1; chunk_size < 3; ++chunk_size) {
            auto chunk_str = " chunk = " + std::to_string(chunk_size);
            SECTION("gather(buffer)" + root_str + chunk_str) {
                // Checks when elements are byte-sized
                gather_buffer_kernel<std::byte>(chunk_size, root, comm);

                // Checks when elements are larger than bytes
                gather_buffer_kernel<double>(chunk_size, root, comm);
            }
        }
    }
}
