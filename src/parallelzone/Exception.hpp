#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace pz {

class pz_exception : public std::runtime_error {
    std::string msg;

public:
    pz_exception(const std::string& arg, const char* file, int line) :
      std::runtime_error(arg) {
        std::ostringstream o;
        o << file << ":" << line << ": " << arg;
        msg = o.str();
    }

    ~pz_exception() throw() {}

    const char* what() const throw() { return msg.c_str(); }
};

} // namespace pz
