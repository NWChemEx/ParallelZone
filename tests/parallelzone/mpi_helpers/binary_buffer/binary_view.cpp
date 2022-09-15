#include <catch2/catch.hpp>
#include <parallelzone/mpi_helpers/binary_buffer/binary_view.hpp>

/* Testing Strategy
 *
 * BinaryView is a class used to represent a view of contiguous bytes.
 */

using namespace parallelzone::mpi_helpers;

namespace {

// Factors out getting pointer/size
template<typename T>
auto byte_data(T&& input) {
    using const_pointer = BinaryView::const_pointer;
    using size_type     = BinaryView::size_type;
    using clean_type    = std::decay_t<T>;
    auto p              = reinterpret_cast<const_pointer>(input.data());
    auto n = input.size() * sizeof(typename clean_type::value_type);
    return std::make_tuple(p, n);
}

} // namespace

using test_types = std::tuple<BinaryView, ConstBinaryView>;

TEMPLATE_LIST_TEST_CASE("BinaryView", "", test_types) {
    using test_type = TestType;

    // Test a type which has byte-sized elements
    std::string str0, str1("Hello world");
    auto [p_str1, n_str1] = byte_data(str1);

    // Test a type with elements that are multiple bytes
    std::vector<double> vec0, vec1{1.1, 1.2, 1.3};
    auto [p_vec1, n_vec1] = byte_data(vec1);

    test_type defaulted;
    test_type str_empty(str0.data(), str0.size());
    test_type str_full(str1.data(), str1.size());

    test_type vec_empty(vec0.data(), vec0.size());
    test_type vec_full(vec1.data(), vec1.size());

    SECTION("Types") {
        using value_type    = typename test_type::value_type;
        using pointer       = typename test_type::pointer;
        using const_pointer = typename test_type::const_pointer;

        if constexpr(std::is_same_v<test_type, BinaryView>) {
            STATIC_REQUIRE(std::is_same_v<value_type, std::byte>);
            STATIC_REQUIRE(std::is_same_v<pointer, std::byte*>);
        } else {
            STATIC_REQUIRE(std::is_same_v<value_type, const std::byte>);
            STATIC_REQUIRE(std::is_same_v<pointer, const std::byte*>);
        }
        STATIC_REQUIRE(std::is_same_v<const_pointer, const std::byte*>);
    }

    SECTION("CTors") {
        SECTION("Default") {
            REQUIRE(defaulted.data() == nullptr);
            REQUIRE(defaulted.size() == 0);
        }

        SECTION("Pointer/Size") {
            REQUIRE(str_empty.data() == nullptr);
            REQUIRE(str_empty.size() == 0);
            REQUIRE(str_full.data() == p_str1);
            REQUIRE(str_full.size() == n_str1);
            REQUIRE(vec_empty.data() == nullptr);
            REQUIRE(vec_empty.size() == 0);
            REQUIRE(vec_full.data() == p_vec1);
            REQUIRE(vec_full.size() == n_vec1);
        }

        SECTION("Implicit Conversion BinaryView->ConstBinaryView") {
            // This test actually works whether test_type is BinaryView or
            // ConstBinaryView. N.B. if it's the latter we're actually
            // retesting the copy ctor instead of the implicit conversion

            ConstBinaryView const_defaulted(defaulted);
            ConstBinaryView const_str_empty(str_empty);
            ConstBinaryView const_str_full(str_full);
            ConstBinaryView const_vec_empty(vec_empty);
            ConstBinaryView const_vec_full(vec_full);

            REQUIRE(const_defaulted.data() == defaulted.data());
            REQUIRE(const_defaulted.size() == defaulted.size());
            REQUIRE(const_str_empty.data() == str_empty.data());
            REQUIRE(const_str_empty.size() == str_empty.size());
            REQUIRE(const_str_full.data() == str_full.data());
            REQUIRE(const_str_full.size() == str_full.size());
            REQUIRE(const_vec_empty.data() == vec_empty.data());
            REQUIRE(const_vec_empty.size() == vec_empty.size());
            REQUIRE(const_vec_full.data() == vec_full.data());
            REQUIRE(const_vec_full.size() == vec_full.size());
        }

        SECTION("Copy") {
            // Here we use operator== to check the value, and then for non-
            // empty objects we ensure the copy is shallow

            test_type defaulted_copy(defaulted);
            REQUIRE(defaulted_copy == defaulted);

            test_type str_empty_copy(str_empty);
            REQUIRE(str_empty_copy == str_empty);

            test_type str_full_copy(str_full);
            REQUIRE(str_full_copy == str_full);
            REQUIRE(str_full_copy.data() == str_full.data());

            test_type vec_empty_copy(vec_empty);
            REQUIRE(vec_empty_copy == vec_empty);

            test_type vec_full_copy(vec_full);
            REQUIRE(vec_full_copy == vec_full);
            REQUIRE(vec_full_copy.data() == vec_full.data());
        }

        SECTION("Copy Assignment") {
            // Here we use operator== to check the value, for non-
            // empty objects we ensure the copy is shallow, and we make sure
            // we return *this

            test_type defaulted_copy;
            auto pdefaulted_copy = &(defaulted_copy = defaulted);
            REQUIRE(defaulted_copy == defaulted);
            REQUIRE(pdefaulted_copy == &defaulted_copy);

            test_type str_empty_copy;
            auto pstr_empty_copy = &(str_empty_copy = str_empty);
            REQUIRE(str_empty_copy == str_empty);
            REQUIRE(pstr_empty_copy == &str_empty_copy);

            test_type str_full_copy;
            auto pstr_full_copy = &(str_full_copy = str_full);
            REQUIRE(str_full_copy == str_full);
            REQUIRE(str_full_copy.data() == str_full.data());
            REQUIRE(pstr_full_copy == &str_full_copy);

            test_type vec_empty_copy;
            auto pvec_empty_copy = &(vec_empty_copy = vec_empty);
            REQUIRE(vec_empty_copy == vec_empty);
            REQUIRE(pvec_empty_copy == &vec_empty_copy);

            test_type vec_full_copy;
            auto pvec_full_copy = &(vec_full_copy = vec_full);
            REQUIRE(vec_full_copy == vec_full);
            REQUIRE(vec_full_copy.data() == vec_full.data());
            REQUIRE(pvec_full_copy == &vec_full_copy);
        }

        SECTION("Move") {
            // Here we use operator== to check the value, and then for non-
            // empty objects we ensure the copy is shallow

            test_type defaulted_copy(defaulted);
            test_type defaulted_move(std::move(defaulted));
            REQUIRE(defaulted_copy == defaulted_move);

            test_type str_empty_copy(str_empty);
            test_type str_empty_move(std::move(str_empty));
            REQUIRE(str_empty_copy == str_empty_move);

            test_type str_full_copy(str_full);
            test_type str_full_move(std::move(str_full));
            REQUIRE(str_full_copy == str_full_move);
            REQUIRE(str_full_copy.data() == str_full_move.data());

            test_type vec_empty_copy(vec_empty);
            test_type vec_empty_move(std::move(vec_empty));
            REQUIRE(vec_empty_copy == vec_empty_move);

            test_type vec_full_copy(vec_full);
            test_type vec_full_move(std::move(vec_full));
            REQUIRE(vec_full_copy == vec_full_move);
            REQUIRE(vec_full_copy.data() == vec_full_move.data());
        }
        SECTION("Move Assignment") {
            // Here we use operator== to check the value, then for non-
            // empty objects we ensure the copy is shallow, and finally
            // we make sure it returns *this

            test_type defaulted_copy(defaulted);
            test_type defaulted_move;
            auto pdefaulted_move = &(defaulted_move = std::move(defaulted));
            REQUIRE(defaulted_copy == defaulted_move);
            REQUIRE(pdefaulted_move == &defaulted_move);

            test_type str_empty_copy(str_empty);
            test_type str_empty_move;
            auto pstr_empty_move = &(str_empty_move = std::move(str_empty));
            REQUIRE(str_empty_copy == str_empty_move);
            REQUIRE(pstr_empty_move == &str_empty_move);

            test_type str_full_copy(str_full);
            test_type str_full_move;
            auto pstr_full_move = &(str_full_move = std::move(str_full));
            REQUIRE(str_full_copy == str_full_move);
            REQUIRE(str_full_copy.data() == str_full_move.data());
            REQUIRE(pstr_full_move == &str_full_move);

            test_type vec_empty_copy(vec_empty);
            test_type vec_empty_move;
            auto pvec_empty_move = &(vec_empty_move = std::move(vec_empty));
            REQUIRE(vec_empty_copy == vec_empty_move);
            REQUIRE(pvec_empty_move == &vec_empty_move);

            test_type vec_full_copy(vec_full);
            test_type vec_full_move;
            auto pvec_full_move = &(vec_full_move = std::move(vec_full));
            REQUIRE(vec_full_copy == vec_full_move);
            REQUIRE(vec_full_copy.data() == vec_full_move.data());
            REQUIRE(pvec_full_move == &vec_full_move);
        }
    }

    SECTION("begin()") {
        REQUIRE(defaulted.begin() == nullptr);
        REQUIRE(str_empty.begin() == nullptr);
        REQUIRE(str_full.begin() == p_str1);
        REQUIRE(vec_empty.begin() == nullptr);
        REQUIRE(vec_full.begin() == p_vec1);
    }

    SECTION("begin() const") {
        REQUIRE(std::as_const(defaulted).begin() == nullptr);
        REQUIRE(std::as_const(str_empty).begin() == nullptr);
        REQUIRE(std::as_const(str_full).begin() == p_str1);
        REQUIRE(std::as_const(vec_empty).begin() == nullptr);
        REQUIRE(std::as_const(vec_full).begin() == p_vec1);
    }

    SECTION("end()") {
        REQUIRE(defaulted.end() == nullptr);
        REQUIRE(str_empty.end() == nullptr);
        REQUIRE(str_full.end() == (str_full.data() + str_full.size()));
        REQUIRE(vec_empty.end() == nullptr);
        REQUIRE(vec_full.end() == (vec_full.data() + vec_full.size()));
    }

    SECTION("end() const") {
        REQUIRE(std::as_const(defaulted).end() == nullptr);
        REQUIRE(std::as_const(str_empty).end() == nullptr);
        auto str_end = str_full.data() + str_full.size();
        REQUIRE(std::as_const(str_full).end() == str_end);
        REQUIRE(std::as_const(vec_empty).end() == nullptr);
        auto vec_end = vec_full.data() + vec_full.size();
        REQUIRE(std::as_const(vec_full).end() == vec_end);
    }

    SECTION("data()") {
        REQUIRE(defaulted.data() == nullptr);
        REQUIRE(str_empty.data() == nullptr);
        REQUIRE(str_full.data() == p_str1);
        REQUIRE(vec_empty.data() == nullptr);
        REQUIRE(vec_full.data() == p_vec1);
    }

    SECTION("data() const") {
        REQUIRE(std::as_const(defaulted).data() == nullptr);
        REQUIRE(std::as_const(str_empty).data() == nullptr);
        REQUIRE(std::as_const(str_full).data() == p_str1);
        REQUIRE(std::as_const(vec_empty).data() == nullptr);
        REQUIRE(std::as_const(vec_full).data() == p_vec1);
    }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(str_empty.size() == 0);
        REQUIRE(str_full.size() == n_str1);
        REQUIRE(vec_empty.size() == 0);
        REQUIRE(vec_full.size() == n_vec1);
    }

    SECTION("operator==") {
        // There's really only two scenarios:
        // - Same length buffer, with the same bytes
        // - Buffer with different data (either because of length or value)
        // So the tests in this section are highly redundant
        // N.B. We rely on operator== in copy/move so we can't prepare different
        //      instances with the same state by copying.

        // Default vs. ...
        REQUIRE(defaulted == test_type{});
        REQUIRE(defaulted == str_empty);
        REQUIRE_FALSE(defaulted == str_full);
        REQUIRE(defaulted == vec_empty);
        REQUIRE_FALSE(defaulted == vec_full);

        // Empty string vs. ...
        REQUIRE(str_empty == defaulted);
        REQUIRE(str_empty == test_type(str0.data(), str0.size()));
        REQUIRE_FALSE(str_empty == str_full);
        REQUIRE(str_empty == vec_empty);
        REQUIRE_FALSE(str_empty == vec_full);

        // Full string vs. ...
        REQUIRE_FALSE(str_full == defaulted);
        REQUIRE_FALSE(str_full == str_empty);
        REQUIRE(str_full == test_type(str1.data(), str1.size()));
        REQUIRE_FALSE(str_full == vec_empty);
        REQUIRE_FALSE(str_full == vec_full);

        // Empty vector vs. ...
        REQUIRE(vec_empty == defaulted);
        REQUIRE(vec_empty == str_empty);
        REQUIRE_FALSE(vec_empty == str_full);
        REQUIRE(vec_empty == test_type(vec0.data(), vec0.size()));
        REQUIRE_FALSE(vec_empty == vec_full);

        // Full vector vs. ...
        REQUIRE_FALSE(vec_full == defaulted);
        REQUIRE_FALSE(vec_full == str_empty);
        REQUIRE_FALSE(vec_full == str_full);
        REQUIRE_FALSE(vec_full == vec_empty);
        REQUIRE(vec_full == test_type(vec1.data(), vec1.size()));
    }

    SECTION("operator!=") {
        // This just negates operator==. So it suffices to check one that
        // should be true and one that should be false
        REQUIRE_FALSE(vec_empty != defaulted);
        REQUIRE(vec_full != defaulted);
    }
}
