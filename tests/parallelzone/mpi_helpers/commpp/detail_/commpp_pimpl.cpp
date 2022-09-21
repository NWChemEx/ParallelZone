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

#include "../../../test_parallelzone.hpp"
#include <parallelzone/mpi_helpers/commpp/detail_/commpp_pimpl.hpp>

using namespace parallelzone::mpi_helpers;

using pimpl_type = parallelzone::mpi_helpers::detail_::CommPPPIMPL;
using root_type  = pimpl_type::opt_root_t;

namespace {

/* Wraps the process of making a std::vector<T> whose elements are:
 * ```
 * (min + 1) (min + 2) ... (max + 1)
 * ```
 * The plus 1 is to avoid simply having every element be its offset. This
 * function is basically std::iota, but std::iota doesn't work for std::byte.
 */
template<typename T>
auto make_data(std::size_t min, std::size_t max) {
    std::vector<T> data;
    for(std::size_t i = min; i < max; ++i) data.push_back((T)(i + 1));
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
void gather_kernel(std::size_t chunk_size, root_type root, pimpl_type& comm) {
    using reference       = pimpl_type::binary_reference;
    using const_reference = pimpl_type::const_binary_reference;
    using size_type       = std::size_t;

    auto am_i_root = root.has_value() ? comm.me() == *root : true;
    auto data      = make_data<T>(0, chunk_size * comm.size());

    auto begin_offset = comm.me() * chunk_size;
    const_reference data_in(data.data() + begin_offset, chunk_size);

    auto rv = comm.gather(data_in, root);

    if(am_i_root) {
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
void gather_buffer_kernel(std::size_t chunk_size, root_type root,
                          pimpl_type& comm) {
    using reference       = pimpl_type::binary_reference;
    using const_reference = pimpl_type::const_binary_reference;
    using size_type       = std::size_t;

    bool am_i_root = root.has_value() ? comm.me() == *root : true;

    auto n_elems = chunk_size * comm.size();
    auto data    = make_data<T>(0, n_elems);

    std::vector<T> out_buffer(n_elems);
    reference buffer_in(out_buffer.data(), out_buffer.size());

    auto begin_offset = comm.me() * chunk_size;
    const_reference data_in(data.data() + begin_offset, chunk_size);

    comm.gather(data_in, buffer_in, root);

    if(am_i_root) {
        // We need to check that the root process has the correct value
        // and that gather didn't muck with the memory allocation of
        // out_buffer
        auto out_begin = out_buffer.begin();
        REQUIRE(std::equal(data.begin(), data.end(), out_begin));
    } else {
        std::vector<T> corr(comm.size() * chunk_size);
        REQUIRE(out_buffer.size() == corr.size());
        REQUIRE(std::equal(out_buffer.begin(), out_buffer.end(), corr.begin()));
    }
}

template<typename T>
void gatherv_kernel(std::size_t chunk_size, root_type root, pimpl_type& comm) {
    using const_reference = pimpl_type::const_binary_reference;

    const auto me      = comm.me();
    const auto n_ranks = comm.size();
    auto am_i_root     = root.has_value() ? me == *root : true;

    auto my_data = make_data<T>(0, chunk_size + me);

    const_reference binary(my_data.data(), my_data.size());
    auto rv = comm.gatherv(binary, root);

    if(am_i_root) {
        REQUIRE(rv.has_value());
        std::vector<T> corr;
        std::vector<int> sizes_corr; // Sizes (in bytes)
        for(std::size_t rank = 0; rank < n_ranks; ++rank) {
            sizes_corr.push_back((chunk_size + rank) * sizeof(T));
            for(std::size_t i = 0; i < chunk_size + rank; ++i)
                corr.push_back(T(i + 1));
        }

        const_reference binary_corr(corr.data(), corr.size());
        auto begin = binary_corr.begin();

        const auto& data  = rv->first;
        const auto& sizes = rv->second;
        REQUIRE(data.size() == binary_corr.size());
        REQUIRE(std::equal(data.begin(), data.end(), begin));
        REQUIRE(sizes == sizes_corr);
    } else {
        REQUIRE_FALSE(rv.has_value());
    }
}

} // namespace

TEST_CASE("CommPPPIMPL") {
    auto& world = testing::PZEnvironment::comm_world();

    pimpl_type comm(world.mpi_comm());
    auto n_ranks = comm.size();
    auto me      = comm.me();

    SECTION("clone()") {
        auto copy = comm.clone();
        REQUIRE(*copy == comm);
    }

    SECTION("comm()") {
        int result;
        MPI_Comm_compare(comm.comm(), MPI_COMM_WORLD, &result);
        REQUIRE(result == MPI_IDENT);
    }

    SECTION("size()") {
        int corr = 0;
        MPI_Comm_size(comm.comm(), &corr);
        REQUIRE(n_ranks == corr);
    }

    SECTION("me()") {
        int corr = 0;
        MPI_Comm_rank(comm.comm(), &corr);
        REQUIRE(me == corr);
    }

    // These loops test various MPI operations under different roots and
    // different message sizes.

    for(std::size_t chunk_size = 1; chunk_size < 2; ++chunk_size) {
        auto chunk_str = " chunk = " + std::to_string(chunk_size);

        SECTION("(all) gather" + chunk_str) {
            gather_kernel<std::byte>(chunk_size, std::nullopt, comm);

            gather_kernel<double>(chunk_size, std::nullopt, comm);
        }

        SECTION("(all) gather (buffer)" + chunk_str) {
            gather_buffer_kernel<std::byte>(chunk_size, std::nullopt, comm);

            gather_buffer_kernel<double>(chunk_size, std::nullopt, comm);
        }

        SECTION("(all) gatherv" + chunk_str) {
            gatherv_kernel<std::byte>(chunk_size, std::nullopt, comm);
            gatherv_kernel<double>(chunk_size, std::nullopt, comm);
        }

        for(std::size_t root = 0; root < std::min(n_ranks, 5); ++root) {
            auto root_str = " root = " + std::to_string(root);
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

            SECTION("gatherv" + root_str + chunk_str) {
                gatherv_kernel<std::byte>(chunk_size, root, comm);

                gatherv_kernel<double>(chunk_size, root, comm);
            }
        }
    }
}
