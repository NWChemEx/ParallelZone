#include "parallelzone/hasher.hpp"
#include <catch2/catch.hpp>
#include <functional>
#include <iostream>

using namespace pz;
TEST_CASE("Hashing with BPHash") {
    struct SA {
        int idx;
        std::vector<int> vec;
        void hash(pz::Hasher& h) const { return h(idx, vec); }
    };
    struct SB {
        int idx;
        std::vector<int> vec;
        void hash(pz::Hasher& h) const { return h(idx, vec); }
    };
    SA sa, sa0;
    SB sb, sb0;
    int i              = 1;
    std::vector<int> v = {1, 2, 3};
    sa.vec             = v;
    sa.idx             = i;
    sb.vec             = v;
    sb.idx             = i;
    auto rv            = std::ref(v);
    auto rs            = std::ref(sa);

    REQUIRE(hash_objects(v) == hash_to_string(make_hash(v)));
    REQUIRE(hash_objects(v) == hash_objects(std::vector<int>{1, 2, 3}));
    REQUIRE_FALSE(hash_objects(rv) == hash_objects(std::vector<int>{3, 2, 1}));
    REQUIRE_FALSE(hash_objects(double(i)) == hash_objects(float(i)));
// BPHash adds typeid to the hash when BPHASH_USE_TYPEID is defined.
#ifdef BPHASH_USE_TYPEID
    std::cout << "BPHash uses typeid for hashes. \n";
    REQUIRE_FALSE(hash_objects(rv) == hash_objects(v));
    REQUIRE_FALSE(hash_objects(rs) == hash_objects(i, v));
    REQUIRE_FALSE(hash_objects(sa) == hash_objects(sb));
    REQUIRE_FALSE(hash_objects(sa0) == hash_objects(sb0));
#else
    REQUIRE(hash_objects(rv) == hash_objects(v));
    REQUIRE(hash_objects(rs) == hash_objects(i, v));
    REQUIRE(hash_objects(sa) == hash_objects(sb));
    // Hash of empty objects might still differ even if typeid is not used.
    // REQUIRE(hash_objects(sa0) == hash_objects(sb0));
#endif
}
