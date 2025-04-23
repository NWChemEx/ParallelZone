#include "../catch.hpp"
#include <iostream>
#include <parallelzone/task/argument_wrapper.hpp>
#include <vector>

using namespace parallelzone::task;

TEST_CASE("ArgumentWrapper<T>") {
    SECTION("Ctors") {
        using T = std::vector<int>;
        T value{1, 2, 3};

        ArgumentWrapper<T> arg_wrapper(value);
        auto pdata = arg_wrapper.value().data();
        REQUIRE(arg_wrapper.value() == value);
        REQUIRE(pdata != value.data());

        SECTION("Copy ctor") {
            ArgumentWrapper<T> copies(arg_wrapper);
            REQUIRE(copies.value() == value);
            REQUIRE(copies.value().data() != pdata);
        }

        SECTION("Move ctor") {
            ArgumentWrapper<T> moves(std::move(arg_wrapper));
            REQUIRE(moves.value() == value);
            REQUIRE(moves.value().data() == pdata);
        }

        SECTION("Copy assignment") {
            ArgumentWrapper<T> copies(T{7, 8, 9});
            auto pcopies = &(copies = arg_wrapper);
            REQUIRE(copies.value() == value);
            REQUIRE(copies.value().data() != pdata);
            REQUIRE(pcopies == &copies);
        }

        SECTION("Move assignment") {
            ArgumentWrapper<T> moves(T{7, 8, 9});
            auto pmoves = &(moves = std::move(arg_wrapper));
            REQUIRE(moves.value() == value);
            REQUIRE(moves.value().data() == pdata);
            REQUIRE(pmoves == &moves);
        }
    }

    SECTION("T == int") {
        using T = int;
        SECTION("By value") {
            ArgumentWrapper<T> by_value(1);
            REQUIRE(by_value.value() == 1);
            REQUIRE(std::as_const(by_value).value() == 1);
        }

        SECTION("By reference") {
            int value = 2;
            ArgumentWrapper<T&> by_ref(value);
            REQUIRE(&by_ref.value() == &value);
            REQUIRE(&std::as_const(by_ref).value() == &value);
        }

        SECTION("By const reference") {
            int value = 2;
            ArgumentWrapper<const T&> by_cref(value);
            REQUIRE(&by_cref.value() == &value);
            REQUIRE(&std::as_const(by_cref).value() == &value);
        }

        SECTION("By rvalue reference") {
            int value = 2;
            ArgumentWrapper<T&&> by_rref(std::move(value));
            REQUIRE(by_rref.value() == value);
            REQUIRE(std::as_const(by_rref).value() == value);
        }
    }

    SECTION("T == std::vector<int>") {
        using T = std::vector<int>;
        T value{1, 2, 3};

        SECTION("By value") {
            ArgumentWrapper<T> by_value(T{1, 2, 3});
            REQUIRE(by_value.value() == value);
            REQUIRE(std::as_const(by_value).value() == value);
        }

        SECTION("By reference") {
            ArgumentWrapper<T&> by_ref(value);
            REQUIRE(&by_ref.value() == &value);
            REQUIRE(&std::as_const(by_ref).value() == &value);
        }

        SECTION("By const reference") {
            ArgumentWrapper<const T&> by_cref(value);
            REQUIRE(&by_cref.value() == &value);
            REQUIRE(&std::as_const(by_cref).value() == &value);
        }

        SECTION("By rvalue reference") {
            auto pvalue = value.data();
            ArgumentWrapper<T&&> by_rref(std::move(value));
            REQUIRE(by_rref.value().data() == pvalue);
            REQUIRE(std::as_const(by_rref).value().data() == pvalue);
        }
    }
}