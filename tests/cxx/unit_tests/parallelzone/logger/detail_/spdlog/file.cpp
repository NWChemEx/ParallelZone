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
#include <filesystem>
#include <parallelzone/logger/detail_/spdlog/file.hpp>
#include <parallelzone/logger/detail_/spdlog/stdout.hpp>

using namespace parallelzone::detail_;

using severity = parallelzone::Logger::severity;

TEST_CASE("FileSpdlog") {
    // Make sure the file doesn't exist before we do this test. We don't want
    // to mess things up if it does.
    if(!std::filesystem::exists("file1.txt")) {
        {
            FileSpdlog file1("file 1", "file1.txt");

            SECTION("clone") { REQUIRE(file1.clone()->are_equal(file1)); }

            SECTION("are_equal") {
                // Same
                REQUIRE(file1.are_equal(FileSpdlog("file 1", "file1.txt")));

                // Different names
                FileSpdlog file12("file 2", "file1.txt");
                REQUIRE_FALSE(file12.are_equal(file1));

                // Different files
                if(!std::filesystem::exists("file2.txt")) {
                    FileSpdlog file2("file 1", "file2.txt");
                    REQUIRE_FALSE(file1.are_equal(file2));
                    std::filesystem::remove("file2.txt");
                }

                // Different derived class
                StdoutSpdlog log0("log 0");
                REQUIRE_FALSE(log0.are_equal(file1));
                REQUIRE_FALSE(file1.are_equal(log0));
            }
        }
        std::filesystem::remove("file1.txt");
    }
}