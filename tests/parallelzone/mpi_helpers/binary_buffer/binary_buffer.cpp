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

#include <catch2/catch.hpp>
#include <parallelzone/mpi_helpers/binary_buffer/binary_buffer.hpp>

/* Testing Strategy:
 *
 * The PIMPLs for this class are tested with a variety of buffer backends and
 * known to work. Hence we only test with a std::vector<std::byte> as the
 * underlying buffer. Also of note, we create our initial instances by making
 * the PIMPLs directly (as opposed to going through the make_binary_buffer
 * convenience function); this is because we need to test the BinaryBuffer
 * class before we can test the convenience functions. Users should use the
 * convenience functions and not construct PIMPLs directly.
 */

using namespace parallelzone::mpi_helpers;

TEST_CASE("BinaryBuffer") {
    using buffer_type = std::vector<std::byte>;
    using pimpl_type  = detail_::BinaryBufferPIMPL<buffer_type>;

    buffer_type empty_buffer;
    buffer_type non_empty_buffer{std::byte{1}, std::byte{2}};

    BinaryBuffer defaulted;
    BinaryBuffer empty(std::make_unique<pimpl_type>(empty_buffer));
    BinaryBuffer non_empty(std::make_unique<pimpl_type>(non_empty_buffer));

    SECTION("CTors") {
        SECTION("Default") {
            REQUIRE(defaulted.data() == nullptr);
            REQUIRE(defaulted.size() == 0);
        }

        SECTION("size") {
            buffer_type corr(10);
            BinaryBuffer bb(10);
            REQUIRE(corr.size() == 10);
            REQUIRE(std::equal(bb.begin(), bb.end(), corr.begin()));
        }

        SECTION("PIMPL") {
            REQUIRE(empty.data() == nullptr);
            REQUIRE(empty.size() == 0);

            REQUIRE(non_empty.size() == non_empty_buffer.size());
            REQUIRE(std::equal(non_empty.begin(), non_empty.end(),
                               non_empty_buffer.data()));
        }

        SECTION("Copy") {
            BinaryBuffer empty_copy(empty);
            // Correct value?
            REQUIRE(empty == empty_copy);

            BinaryBuffer non_empty_copy(non_empty);

            // Is copy?
            REQUIRE(non_empty.data() != non_empty_copy.data());

            // Correct value?
            REQUIRE(non_empty == non_empty_copy);
        }

        SECTION("Copy Assignment") {
            BinaryBuffer empty_copy;
            auto pempty_copy = &(empty_copy = empty);

            // Returns *this?
            REQUIRE(pempty_copy == &empty_copy);

            // Correct value?
            REQUIRE(empty == empty_copy);

            BinaryBuffer non_empty_copy;
            auto pnon_empty_copy = &(non_empty_copy = non_empty);

            // Return *this?
            REQUIRE(pnon_empty_copy == &non_empty_copy);

            // Was copied and not moved?
            REQUIRE(non_empty.data() != non_empty_copy.data());

            // Correct value?
            REQUIRE(non_empty == non_empty_copy);
        }

        SECTION("Move") {
            BinaryBuffer empty_copy(empty);
            BinaryBuffer empty_move(std::move(empty));

            // Correct value?
            REQUIRE(empty_move == empty_copy);

            BinaryBuffer non_empty_copy(non_empty);
            auto pnon_empty = non_empty.data();
            BinaryBuffer non_empty_move(std::move(non_empty));

            // Correct value?
            REQUIRE(non_empty_move == non_empty_copy);

            // Was moved and not copied?
            REQUIRE(non_empty_move.data() == pnon_empty);
        }

        SECTION("Move Assignment") {
            BinaryBuffer empty_copy(empty);
            BinaryBuffer empty_move;
            auto pempty_move = &(empty_move = std::move(empty));

            // Returns *this?
            REQUIRE(pempty_move == &empty_move);

            // Correct value?
            REQUIRE(empty_move == empty_copy);

            BinaryBuffer non_empty_copy(non_empty);
            BinaryBuffer non_empty_move;

            auto pnon_empty      = non_empty.data();
            auto pnon_empty_move = &(non_empty_move = std::move(non_empty));

            // Returns *this?
            REQUIRE(pnon_empty_move == &non_empty_move);

            // Correct value?
            REQUIRE(non_empty_move == non_empty_copy);

            // Is moved and not copied?
            REQUIRE(pnon_empty == non_empty_move.data());
        }
    }

    SECTION("swap") {
        BinaryBuffer empty_copy(empty);
        BinaryBuffer non_empty_copy(non_empty);
        auto p_non_empty_copy = non_empty_copy.data();

        empty_copy.swap(non_empty_copy);

        // empty_copy now contains a copy of non_empty's state
        REQUIRE(empty_copy == non_empty);

        // Pointer is still valid
        REQUIRE(p_non_empty_copy == empty_copy.data());

        // non_empty_copy now contains a copy of empty's state
        REQUIRE(non_empty_copy == empty);
    }

    SECTION("begin()") {
        REQUIRE(empty.begin() == nullptr);
        REQUIRE(non_empty.begin() == non_empty.data());
    }

    SECTION("begin()const") {
        REQUIRE(std::as_const(empty).begin() == nullptr);
        REQUIRE(std::as_const(non_empty).begin() == non_empty.data());
    }

    SECTION("end()") {
        REQUIRE(empty.end() == nullptr);
        REQUIRE(non_empty.end() == (non_empty.data() + non_empty.size()));
    }

    SECTION("end() const") {
        REQUIRE(std::as_const(empty).end() == nullptr);
        auto end = non_empty.data() + non_empty.size();
        REQUIRE(std::as_const(non_empty).end() == end);
    }

    SECTION("data()") {
        REQUIRE(empty.data() == nullptr);

        // Comparing to non_empty.begin() is circular, and we copied the buffer
        // so non_empty_buffer.data() != non_empty.data(). We did std::equal
        // in the ctor, so we know the values are right
        REQUIRE(non_empty.data() != nullptr);
    }

    SECTION("data()const") {
        REQUIRE(std::as_const(empty).data() == nullptr);

        // Comparing to non_empty.begin() is circular, and we copied the buffer
        // so non_empty_buffer.data() != non_empty.data(). We did std::equal
        // in the ctor, so we know the values are right.
        REQUIRE(std::as_const(non_empty).data() != nullptr);
    }

    SECTION("size") {
        REQUIRE(empty.size() == 0);
        REQUIRE(non_empty.size() == non_empty_buffer.size());
    }

    SECTION("operator==") {
        // N.B. We can't use copy/move here because tests for copy/move rely
        //      on equality working, i.e., we need to prepare identical states
        //      via the PIMPL ctor.

        REQUIRE(defaulted == BinaryBuffer{});
        REQUIRE(defaulted == empty);
        REQUIRE_FALSE(defaulted == non_empty);

        REQUIRE(empty == defaulted);
        BinaryBuffer empty2(std::make_unique<pimpl_type>(empty_buffer));
        REQUIRE(empty == empty2);
        REQUIRE_FALSE(empty == non_empty);

        REQUIRE_FALSE(non_empty == defaulted);
        REQUIRE_FALSE(non_empty == empty);

        // Same non-empty buffer
        BinaryBuffer non_empty2(std::make_unique<pimpl_type>(non_empty_buffer));
        REQUIRE(non_empty == non_empty2);

        // Different non-empty buffer
        buffer_type non_empty_buffer2{std::byte{3}, std::byte{4}};
        auto pimpl = std::make_unique<pimpl_type>(non_empty_buffer2);
        BinaryBuffer non_empty3(std::move(pimpl));
        REQUIRE_FALSE(non_empty == non_empty3);
    }

    SECTION("operator!=") {
        // This method simply negates operator==, since operator== works, we
        // just need to check one != comparison
        REQUIRE(empty != non_empty);
    }

    SECTION("BinaryView Conversion") {
        BinaryView defaulted_corr;
        REQUIRE(defaulted_corr == BinaryView(defaulted));

        BinaryView empty_corr(empty.data(), empty.size());
        REQUIRE(empty_corr == BinaryView(empty));

        BinaryView non_empty_corr(non_empty.data(), non_empty.size());
        REQUIRE(non_empty_corr == BinaryView(non_empty));
    }

    SECTION("ConstBinaryView Conversion") {
        ConstBinaryView defaulted_corr;
        REQUIRE(defaulted_corr == ConstBinaryView(defaulted));

        ConstBinaryView empty_corr(empty.data(), empty.size());
        REQUIRE(empty_corr == ConstBinaryView(empty));

        ConstBinaryView non_empty_corr(non_empty.data(), non_empty.size());
        REQUIRE(non_empty_corr == ConstBinaryView(non_empty));
    }
}

TEST_CASE("make_binary_buffer") {
    using const_pointer = BinaryBuffer::const_pointer;

    SECTION("Need to serialize") {
        std::vector<std::string> vec_str{"Hello", "World"};
        auto bb = make_binary_buffer(vec_str);

        auto corr = 10 * sizeof(char) + 3 * sizeof(std::size_t);
        REQUIRE(bb.size() == corr);
    }

    std::vector<double> vec_d{1.1, 1.2, 1.3};
    auto pvec_d = reinterpret_cast<const_pointer>(vec_d.data());
    auto corr_n = 3 * sizeof(double);

    SECTION("Don't need to serialize (copy)") {
        auto bb = make_binary_buffer(vec_d);
        REQUIRE(bb.size() == corr_n);
        REQUIRE(bb.data() != pvec_d);
    }
    SECTION("Don't need to serialize (move)") {
        auto bb = make_binary_buffer(std::move(vec_d));
        REQUIRE(bb.size() == corr_n);
        REQUIRE(bb.data() == pvec_d);
    }
}

TEST_CASE("from_binary_buffer") {
    SECTION("Need to serialize") {
        using type = std::vector<std::string>;
        type vec_string{"Hello", "World"};
        auto binary_copy = make_binary_buffer(vec_string);
        auto copy        = from_binary_buffer<type>(binary_copy);
        REQUIRE(copy == vec_string);
    }

    SECTION("Don't need to serialize") {
        using type = std::vector<double>;
        type vec{1.1, 1.2, 1.3};
        auto binary_copy(make_binary_buffer(vec));
        auto copy = from_binary_buffer<type>(binary_copy);
        REQUIRE(copy == vec);
    }
}
