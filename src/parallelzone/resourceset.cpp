#include <parallelzone/resourceset.hpp>

namespace parallelzone {

Resourceset::Resourceset() {
    // PAPI_library_init(PAPI_VER_CURRENT);

    // papi_info = *PAPI_get_hardware_info();
    // this->papi_info = std::cref( PAPI_get_hardware_info() );
    // papi_info( *PAPI_get_hardware_info() )
}

Resourceset::~Resourceset() {}

} // namespace parallelzone
