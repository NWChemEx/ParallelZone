#include <catch2/catch.hpp>
#include <map>
#include <parallelzone/mpi_helpers/traits.hpp>

/* Testing Strategy
 *
 * These unit tests look at the trait NeedsSerialized<T>. The struct by default
 * says that type T needs to be serialized. Users specialize NeedsSerialized
 * for types that do not need to be serialied.
 *
 * The tests rely on two type lists. `false_list` contains types we have
 * specialized NeedsSerialized for, and `true_list` contains some example
 * types that we haven't specialized NeedsSerialized for.
 *
 * As NeedsSerialized is specialized for more types, those types should be
 * added to the `false_list`.
 */

// List of types we do not need to serialize
using false_list =
  std::tuple<std::string, std::vector<double>, std::vector<int>>;

// List of types we haven't been told not to serialize
using true_list = std::tuple<std::map<int, int>, std::vector<std::string>>;

using namespace parallelzone::mpi_helpers;

TEMPLATE_LIST_TEST_CASE("NeedsSerialized(false)", "", false_list) {
    using T = TestType;
    STATIC_REQUIRE_FALSE(needs_serialized_v<T>);
}

TEMPLATE_LIST_TEST_CASE("NeedsSerialized(true)", "", true_list) {
    using T = TestType;
    STATIC_REQUIRE(needs_serialized_v<T>);
}
