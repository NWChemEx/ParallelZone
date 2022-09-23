#include <catch2/catch.hpp>
#include <parallelzone/mpi_helpers/traits/mpi_data_type.hpp>

using namespace parallelzone::mpi_helpers;

#define REGISTER_TYPE(cxx_type, mpi_type)                  \
    STATIC_REQUIRE(mpi_data_type_v<cxx_type> == mpi_type); \
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
