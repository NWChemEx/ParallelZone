/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "detail_/spdlog/stdout.hpp"
#include <parallelzone/logging/logger_factory.hpp>

namespace parallelzone {

LoggerFactory::logger_type LoggerFactory::default_global_logger(
  mpi_rank_type rank) {
    Logger log;
    if(rank == 0) {
        auto pimpl = parallelzone::detail_::StdoutSpdlog("Rank 0");
        Logger(pimpl.clone()).swap(log);
    }
    return log;
}

} // namespace parallelzone
