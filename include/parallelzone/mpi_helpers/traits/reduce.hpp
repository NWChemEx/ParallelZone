#pragma once
#include <functional>
#include <mpi.h>

namespace parallelzone::mpi_helpers {

template<typename T>
using reduction_op = std::function<T(const T&, const T&)>;

template<typename T>
using reduce_return_type = ;

} // namespace parallelzone::mpi_helpers
