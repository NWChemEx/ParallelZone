#pragma once

#include "logger_pimpl.hpp"

namespace parallelzone::detail_ {

// TODO: This can probbaly be made more performance by adding
// a custom extension of std::streambuf that has a nullcall in
// xputs / overflow
class NullLogger : public FileLogger {
    using my_type = NullLogger;

public:
    NullLogger() : FileLogger("/dev/null"){};
    ~NullLogger() noexcept final = default;

    NullLogger(const my_type&)                = default;
    NullLogger(my_type&&) noexcept            = default;
    NullLogger& operator=(const my_type&)     = default;
    NullLogger& operator=(my_type&&) noexcept = default;
};

} // namespace parallelzone::detail_
