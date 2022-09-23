#pragma once
#include <complex>
#include <mpi.h>
#include <type_traits>

namespace parallelzone::mpi_helpers {

/** @brief Primary template for mapping type @p T to it's MPI enum.
 *
 *  Each MPI implementation needs to define MPI enumerations for types like
 *  MPI_DOUBLE and MPI_INT. The type of those enums is up to the implementation.
 *  The primary template is not defined and is selected when @p T does not map
 *  to a type with a known MPI data type enumeration.
 *
 *  @tparam T
 */
template<typename T>
struct MPIDataType;

template<typename T>
struct HasMPIDataType : std::false_type {};

/// Facilitates mapping C++ type @p cxx_type to MPI enumeration @p mpi_type
#define REGISTER_TYPE(cxx_type, mpi_type)                  \
    template<>                                             \
    struct MPIDataType<cxx_type> {                         \
        static constexpr auto value() { return mpi_type; } \
    };                                                     \
    template<>                                             \
    struct HasMPIDataType<cxx_type> : std::true_type {}

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

#undef REGISTER_TYPE

template<typename T>
static constexpr bool has_mpi_data_type_v = HasMPIDataType<T>::value;

template<typename T>
static constexpr auto mpi_data_type_v = MPIDataType<T>::value();

} // namespace parallelzone::mpi_helpers
