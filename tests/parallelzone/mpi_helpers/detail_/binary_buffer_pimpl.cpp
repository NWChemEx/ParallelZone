#include <catch2/catch.hpp>
#include <parallelzone/mpi_helpers/binary_buffer/detail_/binary_buffer_pimpl.hpp>

/* Testing Strategy:
 *
 * There's really two PIMPL classes: BinaryBufferPIMPLBase and
 * BinaryBufferPIMPL. The former is an abstract base class defining the API
 * used to interact with the latter. Here we make instances of BinaryBufferPIMPL
 * and test it through the API of the former. This means that if the methods
 * work correctly we know that the base class properly forwards and that the
 * derived class implements things correctly.
 */

using namespace parallelzone::mpi_helpers::detail_;

using test_types =
  std::tuple<std::string, std::vector<std::byte>, std::vector<double>>;

namespace {

template<typename T>
T initialize_buffer() {
    if constexpr(std::is_same_v<T, std::string>) {
        return std::string("Hello World");
    } else if constexpr(std::is_same_v<T, std::vector<std::byte>>) {
        return std::vector<std::byte>{std::byte{1}, std::byte{2}};
    } else if constexpr(std::is_same_v<T, std::vector<double>>) {
        return std::vector<double>{1.23, 3.14};
    } else {
        // This static assert will be false if we get here
        static_assert(std::is_same_v<T, std::string>,
                      "Did you add a new type to test_types and forget to add "
                      "an initializer>");
    }
}

} // namespace

TEMPLATE_LIST_TEST_CASE("BinaryBufferPIMPL", "", test_types) {
    using my_type = BinaryBufferPIMPL<TestType>;
    using pointer = typename my_type::const_pointer;

    TestType empty_buffer;
    auto non_empty_buffer = initialize_buffer<TestType>();

    my_type empty(empty_buffer);
    my_type non_empty(non_empty_buffer);

    auto p = reinterpret_cast<pointer>(non_empty_buffer.data());
    auto n = non_empty_buffer.size() * sizeof(typename TestType::value_type);

    SECTION("CTor") {
        // empty and non_empty have been created by copying the buffer into
        // them. Here we test that we can also move the buffer into the new
        // instance

        my_type non_empty_move(std::move(non_empty_buffer));

        // Apparently moving a std::string invalidates pointers...
        if constexpr(!std::is_same_v<TestType, std::string>) {
            REQUIRE(non_empty_move.data() == p);
            REQUIRE(std::equal(p, p + n, non_empty_move.data()));
        }
        REQUIRE(non_empty_move.size() == n);
    }

    SECTION("Deleted ctors/assignment ops") {
        // These shouldn't compile, un-comment to test

        // my_type x0(std::move(empty));
        // empty = non_empty;
        // empty = std::move(non_empty);
    }

    SECTION("clone") {
        auto empty2 = empty.clone();
        REQUIRE(empty2->size() == empty.size());
        REQUIRE(empty2->data() == nullptr);

        auto non_empty2 = non_empty.clone();
        REQUIRE(non_empty2->size() == non_empty.size());   // Same size?
        REQUIRE(non_empty2->data() != non_empty.data());   // Is deep?
        REQUIRE(std::equal(p, p + n, non_empty2->data())); // Same values?
    }

    SECTION("data()") {
        REQUIRE(empty.data() == nullptr);
        REQUIRE(std::equal(p, p + n, non_empty.data()));
    }

    SECTION("data() const") {
        REQUIRE(std::as_const(empty).data() == nullptr);
        REQUIRE(std::equal(p, p + n, std::as_const(non_empty).data()));
    }

    SECTION("size") {
        REQUIRE(empty.size() == 0);
        REQUIRE(non_empty.size() == n);
    }
}
