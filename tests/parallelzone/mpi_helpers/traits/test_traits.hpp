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
#include <map>
#include <parallelzone/mpi_helpers/binary_buffer/binary_buffer.hpp>
#include <vector>

namespace testing {

// List of types we do not need to serialize
using false_list =
  std::tuple<std::string, std::vector<double>, std::vector<int>,
             parallelzone::mpi_helpers::BinaryBuffer,
             parallelzone::mpi_helpers::BinaryView,
             parallelzone::mpi_helpers::ConstBinaryView>;

// List of types we haven't been told not to serialize
using true_list = std::tuple<std::map<int, int>, std::vector<std::string>,
                             std::vector<std::vector<int>>>;

using have_mpi_data_type_list =
  std::tuple<char, signed short, short, signed int, int, signed long, long,
             signed char, char, unsigned char, unsigned short, unsigned int,
             unsigned long int, float, double, long double, bool,
             std::complex<float>, std::complex<double>,
             std::complex<long double>>;

} // namespace testing
