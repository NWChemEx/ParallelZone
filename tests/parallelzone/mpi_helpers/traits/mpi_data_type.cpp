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
#include <parallelzone/mpi_helpers/traits/mpi_data_type.hpp>

using namespace parallelzone::mpi_helpers;

#define REGISTER_TYPE(cxx_type, mpi_type)                                 \
    STATIC_REQUIRE(mpi_data_type_v<cxx_type> == mpi_type);                \
    STATIC_REQUIRE(                                                       \
      std::is_same_v<enable_if_has_mpi_data_type_t<cxx_type, int>, int>); \
    STATIC_REQUIRE(has_mpi_data_type_v<cxx_type>)

TEST_CASE("MPIDataType") {
    REGISTER_TYPE(char, MPI_CHAR);
    REGISTER_TYPE(signed short, MPI_SHORT);
    REGISTER_TYPE(signed int, MPI_INT);
    REGISTER_TYPE(signed long, MPI_LONG);
    REGISTER_TYPE(signed char, MPI_SIGNED_CHAR);
    REGISTER_TYPE(unsigned char, MPI_UNSIGNED_CHAR);
    REGISTER_TYPE(unsigned short, MPI_UNSIGNED_SHORT);
    REGISTER_TYPE(unsigned int, MPI_UNSIGNED);
    REGISTER_TYPE(unsigned long int, MPI_UNSIGNED_LONG);
    REGISTER_TYPE(float, MPI_FLOAT);
    REGISTER_TYPE(double, MPI_DOUBLE);
    REGISTER_TYPE(long double, MPI_LONG_DOUBLE);
    REGISTER_TYPE(bool, MPI_C_BOOL);
    REGISTER_TYPE(std::complex<float>, MPI_C_FLOAT_COMPLEX);
    REGISTER_TYPE(std::complex<double>, MPI_C_DOUBLE_COMPLEX);
    REGISTER_TYPE(std::complex<long double>, MPI_C_LONG_DOUBLE_COMPLEX);
    REGISTER_TYPE(std::byte, MPI_BYTE);
}

#undef REGISTER_TYPE
