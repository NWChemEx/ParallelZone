################################################################################
#
# The master version of this file lives at
# NWChemEx-Project/DeveloperTools/cmake/build_device.cmake. The version contained in
# all other NWChemEx-Project repositories  is a synchronized copy of the master
# version. Any changes made to the non-master version will be lost the next time
# the master version is updated. To make changes please make them to the master
# version.
#
################################################################################

include_guard()

function(build_device)

    # language settings for CUDA
    if( BUILD_CUDA_BINDINGS )
      enable_language( CUDA )
      find_package( CUDAToolkit REQUIRED )

      if(NOT DEFINED CMAKE_CUDA_STANDARD)
        set(CMAKE_CUDA_STANDARD 11)
        set(CMAKE_CUDA_STANDARD_REQUIRED ON)
      endif()
    endif( BUILD_CUDA_BINDINGS )

    # language settings for HIP
    if( BUILD_HIP_BINDINGS )
      enable_language( HIP )
    endif()

    # language settings for SYCL
    if( BUILD_SYCL_BINDINGS )
      unset(CMAKE_CXX_STANDARD PARENT_SCOPE)
      set(CMAKE_CXX_STANDARD 17)
    endif()

endfunction()       

# Call the function we just wrote to get CMakePP
build_device()
