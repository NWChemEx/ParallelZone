// #include <catch2/catch.hpp>
// #include <parallelzone/mpi_helpers/detail_/aliasing_binary_view_pimpl.hpp>

// TEST_CASE("AliasingBinaryViewPIMPL") {
//     using pimpl_type =
//       parallelzone::mpi_helpers::detail_::AliasingBinaryViewPIMPL;

//     using const_byte_pointer = typename pimpl_type::const_byte_pointer;
//     using size_type          = typename pimpl_type::size_type;

//     SECTION("Aliases empty buffer") {
//         std::string buffer;
//         const auto* p = reinterpret_cast<const_byte_pointer>(buffer.data());
//         size_type n   = buffer.size() * sizeof(std::string::value_type);
//         pimpl_type empty(p, n);
//         REQUIRE(empty.data() == p);
//         REQUIRE(empty.size() == n);
//     }

//     SECTION("Aliases non-empty buffer") {
//         std::string buffer("hello world");
//         const auto* p = reinterpret_cast<const_byte_pointer>(buffer.data());
//         size_type n   = buffer.size() * sizeof(std::string::value_type);
//         pimpl_type non_empty(p, n);
//         REQUIRE(non_empty.data() == p);
//         REQUIRE(non_empty.size() == n);
//     }
// }
