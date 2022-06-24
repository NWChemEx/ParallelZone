#pragma once

#include <memory>

namespace parallelzone {

namespace detail_ {
template<typename GPUType>
class GpuPIMPL;
} // namespace detail_

/**
 * @brief Class GPU
 * @details Supports as a convinence wrapper around GPU-vendor APIs
 */
class GPU {
public:
    /// Type of the PIMPL associated with this instance
    using pimpl_type = detail_::GpuPIMPL<>;

    /**
     * @brief Default constructor
     * @details Initializes PAPI and sets up Event sets
     */
    GPU();

private:
    using pimpl_pointer = std::unique_ptr<pimpl_type>;
    pimpl_pointer m_pimpl_;
};

} // namespace parallelzone
