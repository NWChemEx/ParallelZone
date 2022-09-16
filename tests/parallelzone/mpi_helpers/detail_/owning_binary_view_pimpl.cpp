// #include <catch2/catch.hpp>
// #include <parallelzone/mpi_helpers/detail_/owning_binary_view_pimpl.hpp>

// /* Testing Strategy:
//  *
//  * At the moment, we are primarily targeting std::string as a binary buffer.
//  * So these tests only focus on it. If additional binary buffers are used they
//  * should be tested here too.
//  */

// template<typename T>
// using pimpl_type = parallelzone::mpi_helpers::detail_::OwningBinaryViewPIMPL<T>;

// namespace {

// template<typename T>
// auto corr(T&& input) {
//     using clean_t            = std::decay_t<T>;
//     using const_byte_pointer = typename pimpl_type<clean_t>::const_byte_pointer;
//     using size_type          = typename pimpl_type<clean_t>::size_type;
//     using value_type         = typename clean_t::value_type;

//     const auto* p = reinterpret_cast<const_byte_pointer>(input.data());
//     const auto n  = input.size() * sizeof(value_type);
//     return std::make_tuple(p, n);
// }

// } // namespace

// TEST_CASE("OwningBinaryViewPIMPL") {
//     SECTION("T == std::string") {
//         using T = std::string;

//         SECTION("empty") {
//             T buffer;
//             pimpl_type<T> p(buffer);
//             auto [p_corr, n_corr] = corr(buffer);
//             REQUIRE(p.size() == n_corr);
//             REQUIRE(std::equal(p_corr, p_corr + n_corr, p.data()));
//         }

//         SECTION("value") {
//             T buffer("Hello World");
//             pimpl_type<T> p(buffer);
//             auto [p_corr, n_corr] = corr(buffer);
//             REQUIRE(p.size() == n_corr);
//             REQUIRE(std::equal(p_corr, p_corr + n_corr, p.data()));
//         }
//     }
// }
