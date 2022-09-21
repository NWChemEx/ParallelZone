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

#include "../../test_parallelzone.hpp"
#include <parallelzone/mpi_helpers/commpp/commpp.hpp>

using namespace parallelzone::mpi_helpers;

namespace {} // namespace

TEST_CASE("CommPP") {
    using value_type      = CommPP::binary_type;
    using const_reference = CommPP::const_binary_reference;

    auto& world = testing::PZEnvironment::comm_world();

    CommPP defaulted;
    CommPP comm(world.mpi_comm());
    auto n_ranks = comm.size();
    auto me      = comm.me();

    SECTION("CTors") {}

    SECTION("size()") {
        REQUIRE(defaulted.size() == 0);
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

    for(std::size_t root = 0; root < std::min(n_ranks, 5); ++root) {
        auto root_str = " root = " + std::to_string(root);
        for(std::size_t chunk_size = 1; chunk_size < 5; ++chunk_size) {
            auto chunk_str = " chunk = " + std::to_string(chunk_size);

            SECTION("gather (serialize)") {
                using vec_type = std::vector<std::string>;
                auto max_elems = comm.size() * chunk_size;

                auto begin_value = comm.me() * chunk_size;
                auto end_value   = (comm.me() + 1) * chunk_size;

                vec_type my_data;
                for(std::size_t i = begin_value; i < end_value; ++i)
                    my_data.push_back("Hello " + std::to_string(i));

                auto rv = comm.gather(std::move(my_data), root);

                if(comm.me() == root) {
                    std::vector<vec_type> corr;
                    for(std::size_t i = 0; i < comm.size(); ++i) {
                        vec_type i_data;
                        begin_value = i * chunk_size;
                        end_value   = (i + 1) * chunk_size;
                        for(std::size_t j = begin_value; j < end_value; ++j) {
                            i_data.push_back("Hello " + std::to_string(j));
                        }
                        corr.push_back(i_data);
                    }

                    REQUIRE(rv.has_value());
                    REQUIRE(*rv == corr);
                } else {
                    REQUIRE_FALSE(rv.has_value());
                }
            }
            SECTION("gather (no_serialize)") {
                using vec_type = std::vector<double>;
                auto max_elems = comm.size() * chunk_size;

                auto begin_value = comm.me() * chunk_size;
                auto end_value   = (comm.me() + 1) * chunk_size;

                vec_type my_data;
                for(std::size_t i = begin_value; i < end_value; ++i)
                    my_data.push_back(double(i));

                auto rv = comm.gather(std::move(my_data), root);

                if(comm.me() == root) {
                    vec_type corr;
                    for(std::size_t i = 0; i < max_elems; ++i) {
                        corr.push_back(double(i));
                    }
                    REQUIRE(rv.has_value());
                    REQUIRE(*rv == corr);
                } else {
                    REQUIRE_FALSE(rv.has_value());
                }
            }
        }
    }
}
