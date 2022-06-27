#pragma once

#include <array>
#include <memory>

namespace parallelzone {

  namespace detail_ {
    template <size_t EventSize=32>
    class PapiPIMPL;
  } // namespace detail_

/**
 * @brief Class Papi
 * @details Supports as a convinence wrapper around PAPI
 */
class Papi {
public:
    /// Type of the PIMPL associated with this instance
    using pimpl_type = detail_::PapiPIMPL<>;

    /**
     * @brief Default constructor
     * @details Initializes PAPI and sets up Event sets
     */
    Papi();

    /**
     * @brief destructor
     * @details Destructs the PAPI appropriately
     */
    ~Papi();

    /**
     * @details Start event measurement
     */
    void start_measurement();

    /**
     * @details Stop event measurement
     */
    void stop_measurement();

    /**
     * @details resets event set measurement
     */
    void reset_measurement() noexcept;

    Papi(const Papi&) = delete;
    Papi& operator=(const Papi&) = delete;
    Papi(Papi&&)                 = delete;
    Papi& operator=(Papi&&) = delete;

private:
    using pimpl_pointer = std::unique_ptr<pimpl_type>;
    pimpl_pointer m_pimpl_;
};

} // namespace parallelzone
