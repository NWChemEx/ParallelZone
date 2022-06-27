#pragma once

#include <utility>

namespace parallelzone {

/** @brief CPU class enables serialization and deserialization of
 * objects using various types of archives supported by MADNESS and Cereal.
 */
  struct CPU : Processor, public Storage {
  public:
    CPU()                      = default;
    ~CPU() override            = default;
    CPU(CPU const&)            = default;
    CPU(CPU&&)                 = default;
    CPU& operator=(CPU const&) = default;
    CPU& operator=(CPU&&)      = default;

  private:
    /**
     * @brief Get free and available memory for memory resource
     *
     * @return std::pair containg free_size and total_size of memory
     */
    [[nodiscard]] std::pair<std::size_t, std::size_t> do_get_mem_info() const override
      {
	std::size_t free_size{};
	std::size_t total_size{};
	
      }
  };

} // namespace parallelzone
