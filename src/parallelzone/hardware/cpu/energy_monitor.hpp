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

#pragma once
#ifdef BUILD_CPP_JOULES
#include <cppJoules.h>

namespace parallelzone::hardware {

class EnergyMonitor {
public:
    using energy_type = long long;
    bool is_active() { return true; }
    void start() { m_tracker_.start(); }

    void stop() {
        m_tracker_.stop();
        m_tracker_.calculate_energy();
        m_tracker_.print_energy();
    }

private:
    EnergyTracker m_tracker_;
};

} // namespace parallelzone::hardware

#else
namespace parallelzone::hardware {

class EnergyMonitor {
public:
    bool is_active() { return false; }
    void start() {}
    void stop() {}
};

} // namespace parallelzone::hardware

#endif