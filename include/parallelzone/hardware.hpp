#pragma once

// #include <parallelzone/storage.hpp>
// #include <parallelzone/processor.hpp>
#include <string>

namespace parallelzone {

template <class DevType>
class Hardware {
public:
  /**
   * @brief Default constructor
   * @details Hardware instance
   */
  Hardware(std::string& name_id) : name(name_id) {}

  std::string& get_name() {return name;}
  
private:
  std::string& name;
  // Storage<DevType>& storage;
  // Processor<DevType>& processor;
};

  template class Hardware<CPU>;
  template class Hardware<GPU>;    

} // End namespace parallelzone
