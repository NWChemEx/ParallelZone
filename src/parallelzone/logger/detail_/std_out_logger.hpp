#pragma once

#include "logger_pimpl.hpp"

namespace parallelzone::detail_ {

class StdOutLogger : public detail_::LoggerPIMPL {
    using base_type = detail_::LoggerPIMPL;
    using base_ptr  = std::unique_ptr<base_type>;
    using my_type   = StdOutLogger;

public:
    StdOutLogger()                 = default;
    ~StdOutLogger() noexcept final = default;

    StdOutLogger(const my_type&)                = default;
    StdOutLogger(my_type&&) noexcept            = default;
    StdOutLogger& operator=(const my_type&)     = default;
    StdOutLogger& operator=(my_type&&) noexcept = default;

    std::ostream& stream_() final { return std::cout; }
    base_ptr clone_() const final { return std::make_unique<my_type>(); }
};

} // namespace parallelzone::detail_
