#pragma once
#ifdef BUILD_CPP_JOULES
#include <cppJoules.h>

namespace parallelzone::hardware {

class EnergyMonitor{
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

}

#else
namespace parallelzone::hardware {

class EnergyMonitor {
public:
    bool is_active() { return false; }
    void start() {}
    void stop() {}
};

}

#endif