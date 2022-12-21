// /*
//  * Copyright 2022 NWChemEx-Project
//  *
//  * Licensed under the Apache License, Version 2.0 (the "License");
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at
//  *
//  * http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, software
//  * distributed under the License is distributed on an "AS IS" BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  */

// #include "parallelzone/logger/logger.hpp"
// #include <catch2/catch.hpp>
// #include <fstream>
// #include <iostream>
// #include <sstream>

// // XXX This Leaks MPI!!
// #include <mpi.h>
// #define ROOT_ONLY(c)         \
//     int rank;                \
//     MPI_Comm_rank(c, &rank); \
//     if(!rank) return;

// TEST_CASE("STDOUT Logger", "[logger][stdout]") {
//     ROOT_ONLY(MPI_COMM_WORLD);
//     auto logger = parallelzone::make_stdout_logger();

//     // Redirect STDOUT to string
//     std::stringstream str;
//     auto cout_rdbuf = std::cout.rdbuf(str.rdbuf());
//     logger.stream() << "Hello world" << std::endl;

//     // Reser STDOUT
//     std::cout.rdbuf(cout_rdbuf);
//     REQUIRE(str.str() == "Hello world\n");
// }

// TEST_CASE("STDERR Logger", "[logger][stderr]") {
//     ROOT_ONLY(MPI_COMM_WORLD);
//     auto logger = parallelzone::make_stderr_logger();

//     // Redirect STDOUT to string
//     std::stringstream str;
//     auto cerr_rdbuf = std::cerr.rdbuf(str.rdbuf());
//     logger.stream() << "Hello world" << std::endl;

//     // Reser STDOUT
//     std::cerr.rdbuf(cerr_rdbuf);
//     REQUIRE(str.str() == "Hello world\n");
// }

// TEST_CASE("File Logger", "[logger][file]") {
//     ROOT_ONLY(MPI_COMM_WORLD);
//     SECTION("with file") {
//         // Ensure file close before check
//         {
//             auto logger = parallelzone::make_file_logger("my_test.txt");
//             logger.stream() << "Hello world" << std::endl;
//         }

//         {
//             std::ifstream file("my_test.txt");
//             std::stringstream buff;
//             buff << file.rdbuf();
//             REQUIRE(buff.str() == "Hello world\n");
//         }

//         std::remove("my_test.txt");
//     }

//     SECTION("no copy") {
//         auto logger = parallelzone::make_file_logger("my_test.txt");
//         REQUIRE_THROWS_AS(parallelzone::Logger(logger), std::runtime_error);
//         std::remove("my_test.txt");
//     }
// }

// TEST_CASE("Null Logger", "[logger][null]") {
//     ROOT_ONLY(MPI_COMM_WORLD);
//     // Just check that we're calling into the void?
//     auto logger = parallelzone::make_null_logger();
//     logger.stream() << "IF YOU'RE SEEING THIS, ITS WRONG" << std::flush;
// }

// TEST_CASE("Stream Logger", "[logger][stream]") {
//     ROOT_ONLY(MPI_COMM_WORLD);
//     SECTION("String Stream") {
//         std::stringstream ss;
//         auto logger = parallelzone::make_stream_logger(&ss);
//         logger.stream() << "Hello world" << std::endl;
//         REQUIRE(ss.str() == "Hello world\n");
//     }

//     SECTION("Null") {
//         auto logger = parallelzone::make_stream_logger(nullptr);
//         REQUIRE_THROWS_AS(logger.stream(), std::runtime_error);
//     }
// }
