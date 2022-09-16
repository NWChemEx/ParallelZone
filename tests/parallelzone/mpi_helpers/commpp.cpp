/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../test_parallelzone.hpp"
#include <parallelzone/mpi_helpers/commpp.hpp>

using namespace parallelzone::mpi_helpers;

namespace {

template<typename T>
auto make_data(std::size_t chunk_size, CommPP& comm) {
    auto max_elems = comm.size() * chunk_size;

    std::vector<T> data;
    for(std::size_t i = 0; i < max_elems; ++i) data.push_back((T)(i + 1));

    return data;
}

/** This kernel tests gathering chunks of size @p chunk_size to process @p root
 *  without a buffer.
 *
 *  The kernel first creates a std::vector<T> `v` with
 *  `chunk_size * comm.size()` elements (unimaginatively the `i`-th element of
 *  `v` is just `i+1`). Next we have rank `r` send elements `v[r*chunk_size]`
 *  through `v[(r+1) * chunk_size - 1]` to rank @p root. Consequently @p root
 *  should get back `v`.
 */
template<typename T>
void gather_kernel(std::size_t chunk_size, std::size_t root, CommPP& comm) {
    using reference       = CommPP::binary_reference;
    using const_reference = CommPP::const_binary_reference;
    using size_type       = std::size_t;

    auto data = make_data<T>(chunk_size, comm);

    auto begin_offset = comm.me() * chunk_size;
    const_reference data_in(data.data() + begin_offset, chunk_size);

    auto rv = comm.gather(data_in, root);

    if(comm.me() == root) {
        REQUIRE(rv.has_value());
        const_reference binary_data(data.data(), data.size());
        REQUIRE(rv->size() == binary_data.size());
        REQUIRE(std::equal(rv->begin(), rv->end(), binary_data.begin()));
    } else {
        REQUIRE_FALSE(rv.has_value());
    }
}

/** This kernel tests gathering chunks of size @p chunk_size to process @p root
 *  when the user supplies a buffer.
 *
 *  This kernel works very similar to gather_kernel except that: we need to
 *  make an output buffer, pass the output buffer, and test that gather
 *  correctly avoided the copy.
 */
template<typename T>
void gather_buffer_kernel(std::size_t chunk_size, std::size_t root,
                          CommPP& comm) {
    using reference       = CommPP::binary_reference;
    using const_reference = CommPP::const_binary_reference;
    using size_type       = std::size_t;

    auto data = make_data<T>(chunk_size, comm);

    std::vector<T> out_buffer(comm.size() * chunk_size);
    auto p = out_buffer.data();
    reference buffer_in(out_buffer.data(), out_buffer.size());

    auto begin_offset = comm.me() * chunk_size;
    const_reference data_in(data.data() + begin_offset, chunk_size);

    comm.gather(data_in, buffer_in, root);

    if(comm.me() == root) {
        // We need to check that the root process has the correct value
        // and that gather didn't muck with the memory allocation of
        // out_buffer

        auto out_begin = out_buffer.begin();
        REQUIRE(std::equal(data.begin(), data.end(), out_begin));
        REQUIRE(p == out_buffer.data());
    } else {
        std::vector<T> corr(comm.size() * chunk_size);
        REQUIRE(out_buffer.size() == corr.size());
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

    for(std::size_t root = 0; root < std::min(n_ranks, 5); ++root) {
        auto root_str = " root = " + std::to_string(root);
        for(std::size_t chunk_size = 1; chunk_size < 5; ++chunk_size) {
            auto chunk_str = " chunk = " + std::to_string(chunk_size);

            SECTION("gather" + root_str + chunk_str) {
                gather_kernel<std::byte>(chunk_size, root, comm);

                gather_kernel<double>(chunk_size, root, comm);
            }

            SECTION("gather(buffer)" + root_str + chunk_str) {
                // Checks when elements are byte-sized
                gather_buffer_kernel<std::byte>(chunk_size, root, comm);

                // Checks when elements are larger than bytes
                gather_buffer_kernel<double>(chunk_size, root, comm);
            }
        }
    }
}
