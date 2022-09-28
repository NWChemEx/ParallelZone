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
using size_type     = std::size_t;
using opt_root_type = std::optional<size_type>;

TEST_CASE("CommPP") {
    using value_type      = CommPP::binary_type;
    using const_reference = CommPP::const_binary_reference;

    auto& world = testing::PZEnvironment::comm_world();

    CommPP defaulted;
    CommPP null(MPI_COMM_NULL);
    CommPP comm(world.mpi_comm());

    int corr_rank, corr_size;
    MPI_Comm_size(comm.comm(), &corr_size);
    MPI_Comm_rank(comm.comm(), &corr_rank);

    auto n_ranks = comm.size();
    auto me      = comm.me();

    SECTION("CTors") {
        SECTION("Default") {
            REQUIRE(defaulted.comm() == MPI_COMM_NULL);
            REQUIRE(defaulted.size() == 0);
            REQUIRE(defaulted.me() == MPI_PROC_NULL);
        }
        SECTION("value") {
            REQUIRE(null.comm() == MPI_COMM_NULL);
            REQUIRE(null.size() == 0);
            REQUIRE(null.me() == MPI_PROC_NULL);

            REQUIRE(comm.comm() == MPI_COMM_WORLD);
            REQUIRE(comm.size() == corr_size);
            REQUIRE(comm.me() == corr_rank);
        }
        SECTION("copy") {
            CommPP copy_defaulted(defaulted);
            REQUIRE(copy_defaulted == defaulted);

            CommPP copy_null(null);
            REQUIRE(copy_null == null);

            CommPP copy_comm(comm);
            REQUIRE(copy_comm == comm);
        }

        SECTION("copy assignment") {
            CommPP copy_defaulted;
            auto pcopy_defaulted = &(copy_defaulted = defaulted);
            REQUIRE(pcopy_defaulted == &copy_defaulted);
            REQUIRE(copy_defaulted == defaulted);

            CommPP copy_null;
            auto pcopy_null = &(copy_null = null);
            REQUIRE(pcopy_null == &copy_null);
            REQUIRE(copy_null == null);

            CommPP copy_comm;
            auto pcomm = &(copy_comm = comm);
            REQUIRE(pcomm == &copy_comm);
            REQUIRE(copy_comm == comm);
        }

        SECTION("move") {
            CommPP copy_defaulted(defaulted);
            CommPP move_defaulted(std::move(defaulted));
            REQUIRE(copy_defaulted == move_defaulted);

            CommPP copy_null(null);
            CommPP move_null(std::move(null));
            REQUIRE(copy_null == move_null);

            CommPP copy_comm(comm);
            CommPP move_comm(std::move(comm));
            REQUIRE(copy_comm == move_comm);
        }

        SECTION("move assignment") {
            CommPP copy_defaulted(defaulted);
            CommPP move_defaulted;
            auto pmove_defaulted = &(move_defaulted = std::move(defaulted));
            REQUIRE(pmove_defaulted == &move_defaulted);
            REQUIRE(copy_defaulted == move_defaulted);

            CommPP copy_null(null);
            CommPP move_null;
            auto pmove_null = &(move_null = std::move(null));
            REQUIRE(pmove_null == &move_null);
            REQUIRE(copy_null == move_null);

            CommPP copy_comm(comm);
            CommPP move_comm;
            auto pmove_comm = &(move_comm = std::move(comm));
            REQUIRE(pmove_comm == &move_comm);
            REQUIRE(copy_comm == move_comm);
        }
    }

    SECTION("comm") {
        REQUIRE(defaulted.comm() == MPI_COMM_NULL);
        REQUIRE(null.comm() == MPI_COMM_NULL);
        REQUIRE(comm.comm() == MPI_COMM_WORLD);
    }

    SECTION("size()") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(null.size() == 0);
        REQUIRE(n_ranks == corr_size);
    }

    SECTION("me()") {
        REQUIRE(defaulted.me() == MPI_PROC_NULL);
        REQUIRE(null.me() == MPI_PROC_NULL);
        REQUIRE(me == corr_rank);
    }

    SECTION("swap") {
        CommPP copy_comm(comm);
        CommPP copy_null(null);

        null.swap(comm);
        REQUIRE(copy_comm == null);
        REQUIRE(copy_null == comm);
    }

    SECTION("operator==") {
        REQUIRE(defaulted == CommPP());
        REQUIRE(defaulted == null);
        REQUIRE_FALSE(defaulted == comm);

        REQUIRE(null == defaulted);
        REQUIRE(null == CommPP(MPI_COMM_NULL));
        REQUIRE_FALSE(null == comm);

        REQUIRE_FALSE(comm == defaulted);
        REQUIRE_FALSE(comm == null);
        REQUIRE(comm == CommPP(MPI_COMM_WORLD));
    }

    SECTION("operator!=") {
        // This method just negates operator==, so testing one false outcome
        // and one true outcome suffices

        REQUIRE_FALSE(defaulted != null);
        REQUIRE(null != comm);
    }

    // These loops test various MPI operations under different roots and
    // different message sizes.

    // All of these types must be constructible given a single std::size_t
    const int max_ranks              = 5;
    const std::size_t max_chunk_size = 5;
    using needs_serialized           = std::string;
    using no_serialization           = double;

    for(std::size_t chunk_size = 1; chunk_size < max_chunk_size; ++chunk_size) {
        auto chunk_str = " chunk = " + std::to_string(chunk_size);
        auto begin     = me * chunk_size;
        auto end       = begin + chunk_size;

        SECTION("all gather" + chunk_str) {
            SECTION("needs serialized") {
                using data_type = std::vector<needs_serialized>;
                data_type local_data(chunk_size, "Hello");
                auto rv = comm.gather(local_data);
                std::vector<data_type> corr(n_ranks, local_data);
                REQUIRE(rv == corr);
            }

            SECTION("doesn't need serialized") {
                using data_type = std::vector<no_serialization>;
                data_type local_data(chunk_size);
                std::iota(local_data.begin(), local_data.end(), begin);
                auto rv = comm.gather(local_data);
                data_type corr(n_ranks * chunk_size);
                std::iota(corr.begin(), corr.end(), 0.0);
                REQUIRE(rv == corr);
            }
        }

        SECTION("all gatherv" + chunk_str) {
            SECTION("needs serialized") {
                using data_type = std::vector<needs_serialized>;
                data_type local_data(chunk_size * me, "Hello");
                auto rv = comm.gatherv(local_data);
                std::vector<data_type> corr;
                for(std::size_t i = 0; i < n_ranks; ++i) {
                    corr.emplace_back(data_type(chunk_size * i, "Hello"));
                }
                REQUIRE(rv == corr);
            }

            SECTION("doesn't need serialized") {
                using data_type = std::vector<no_serialization>;
                data_type local_data(chunk_size * me);
                std::iota(local_data.begin(), local_data.end(), begin);
                auto rv = comm.gatherv(local_data);
                data_type corr;
                for(size_type i = 0; i < n_ranks; ++i) {
                    data_type temp(chunk_size * i);
                    std::iota(temp.begin(), temp.end(), i * chunk_size);
                    for(const auto x : temp) corr.push_back(x);
                }
                REQUIRE(rv == corr);
            }
        }

        SECTION("all reduce" + chunk_str) {
            using data_type = std::vector<no_serialization>;
            data_type local_data(chunk_size);
            std::iota(local_data.begin(), local_data.end(), begin);
            auto rv = comm.reduce(local_data, std::plus<no_serialization>());

            data_type corr(chunk_size);
            for(size_type i = 0; i < n_ranks; ++i) {
                auto begin = i * chunk_size;
                for(size_type j = 0; j < chunk_size; ++j) corr[j] += begin + j;
            }
            REQUIRE(rv == corr);
        }

        for(std::size_t root = 0; root < std::min(n_ranks, max_ranks); ++root) {
            auto root_str = " root = " + std::to_string(root);

            SECTION("gather " + root_str + chunk_str) {
                SECTION("needs serialized") {
                    using data_type = std::vector<needs_serialized>;
                    data_type local_data(chunk_size, "Hello");
                    auto rv = comm.gather(local_data, root);
                    if(me == root) {
                        std::vector<data_type> corr(n_ranks, local_data);
                        REQUIRE(rv.has_value());
                        REQUIRE(*rv == corr);
                    } else {
                        REQUIRE_FALSE(rv.has_value());
                    }
                }

                SECTION("doesn't need serialized") {
                    using data_type = std::vector<no_serialization>;
                    data_type local_data(chunk_size);
                    std::iota(local_data.begin(), local_data.end(), begin);
                    auto rv = comm.gather(local_data, root);
                    if(me == root) {
                        data_type corr(n_ranks * chunk_size);
                        std::iota(corr.begin(), corr.end(), 0.0);
                        REQUIRE(rv.has_value());
                        REQUIRE(*rv == corr);
                    } else {
                        REQUIRE_FALSE(rv.has_value());
                    }
                }
            }

            SECTION("gatherv " + root_str + chunk_str) {
                SECTION("needs serialized") {
                    using data_type = std::vector<needs_serialized>;
                    data_type local_data(chunk_size * me, "Hello");
                    auto rv = comm.gatherv(local_data, root);

                    if(me == root) {
                        std::vector<data_type> corr;
                        for(std::size_t i = 0; i < n_ranks; ++i) {
                            corr.emplace_back(
                              data_type(chunk_size * i, "Hello"));
                        }
                        REQUIRE(rv.has_value());
                        REQUIRE(*rv == corr);
                    } else {
                        REQUIRE_FALSE(rv.has_value());
                    }
                }
                SECTION("doesn't need serialized") {
                    using data_type = std::vector<no_serialization>;
                    data_type local_data(chunk_size * me);
                    std::iota(local_data.begin(), local_data.end(), begin);
                    auto rv = comm.gatherv(local_data, root);
                    if(me == root) {
                        data_type corr;
                        for(size_type i = 0; i < n_ranks; ++i) {
                            data_type temp(chunk_size * i);
                            std::iota(temp.begin(), temp.end(), i * chunk_size);
                            for(const auto x : temp) corr.push_back(x);
                        }
                        REQUIRE(rv.has_value());
                        REQUIRE(*rv == corr);
                    } else {
                        REQUIRE_FALSE(rv.has_value());
                    }
                }
            }
            SECTION("reduce" + root_str + chunk_str) {
                using data_type = std::vector<no_serialization>;
                data_type local_data(chunk_size);
                std::iota(local_data.begin(), local_data.end(), begin);
                auto op = std::plus<no_serialization>();
                auto rv = comm.reduce(local_data, op, root);

                if(me == root) {
                    data_type corr(chunk_size);
                    for(size_type i = 0; i < n_ranks; ++i) {
                        auto begin = i * chunk_size;
                        for(size_type j = 0; j < chunk_size; ++j)
                            corr[j] += begin + j;
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
