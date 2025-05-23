/*
 * Copyright 2025 NWChemEx-Project
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

#include <parallelzone/hardware/cpu/cpu.hpp>
#include "energy_monitor.hpp"
namespace parallelzone::hardware {

typename CPU::profile_return_type CPU::profile_it_(task_type&& task) const {
    profile_information i;
    EnergyMonitor monitor;
    monitor.start();
    const auto t1 = std::chrono::high_resolution_clock::now();
    auto result   = task();
    const auto t2 = std::chrono::high_resolution_clock::now();
    monitor.stop();
    i.wall_time   = (t2 - t1);
    return std::make_pair(std::move(result), std::move(i));
}

} // namespace parallelzone::hardware