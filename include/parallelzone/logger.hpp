#pragma once
#include <ostream>
#include <string>
#include <memory>

namespace parallelzone {
namespace detail_ {
class LoggerPIMPL;
}

class Logger {
    using pimpl_type = detail_::LoggerPIMPL;
    using pimpl_ptr  = std::unique_ptr<pimpl_type>;
public:
    Logger();
    Logger(pimpl_ptr&&);
    ~Logger() noexcept;

    Logger(const Logger&);
    Logger(Logger&&) noexcept;
    Logger& operator=(const Logger&);
    Logger& operator=(Logger&&) noexcept;

    std::ostream& stream();
    Logger& print(const std::string&);
private:
    pimpl_ptr m_pimpl_;
};

template <typename T>
Logger& operator<<(Logger& log, T&& val) {
    log.stream() << val;
    return log;
}

Logger make_stdout_logger();
Logger make_stderr_logger();
Logger make_file_logger(std::string);
Logger make_null_logger();
Logger make_stream_logger(std::ostream*);

}
