#pragma once
#include <string>
#include <vector>

namespace pz {

  // "PAPI_dev_info_u"
  class HardwareComponent {
    int numDevices_ = 0;
    bool is_gpu_;
    std::vector<HWCompStorage*> m_hwComponent;

    struct {
      std::string name;
      int warp_size;
      int max_threads_per_block;
      int max_blocks_per_multi_proc;
      int max_shmmem_per_block;
      int max_block_dim_x;
      int max_block_dim_y;
      int max_block_dim_z;
      int max_grid_dim_x;
      int max_grid_dim_y;
      int max_grid_dim_z;
      int major;
      int minor;
    } devInfo;

  public:
    HardwareComponent(const PAPI_dev_type_info_t& papi_dev_type);

    ~HardwareComponent();

    initialize(const PAPI_dev_type_info_t& papi_dev_type);

    int get_numDevices( return numDevices_ );

    bool is_gpu( return is_gpu_ );

    const std::vector<HWCompStorage*>& get_HWCompStorage( return m_hwComponent );
  };

  //--------------------------------------------------------------------

  HardwareComponent::HardwareComponent(const PAPI_dev_type_info_t& papi_dev_type) {
    this->numDevices_ = papi_dev_type.num_devices;
    this->initialize(papi_dev_type);

    for (int devID = 0; devID < numDevices_; devID++) {
      HWCompStorage* hwcomp_storage = new HWCompStorage();
      this->m_hwComponent.push_back(hwcomp_storage);
    }
  }

  //--------------------------------------------------------------------

  HardwareComponent::initialize(const PAPI_dev_type_info_t& dev_type_info) {
    if ( dev_type_info.id == PAPI_DEV_TYPE_ID__CPU && this->numDevices_ > 0 ) {
      PAPI_cpu_info_t *cpu_info = (PAPI_cpu_info_t *) dev_type_info.dev_info_arr;
      this->is_gpu_ = false;

      devInfo.name                      = cpu_info->name;
      devInfo.warp_size                 = -1;
      devInfo.max_threads_per_block     = -1;
      devInfo.max_blocks_per_multi_proc = -1;
      devInfo.max_shmem_per_multi_proc  = -1;
      devInfo.max_block_dim_x           = -1;
      devInfo.max_block_dim_y           = -1;
      devInfo.max_block_dim_z           = -1;
      devInfo.max_grid_dim_x            = -1;
      devInfo.max_grid_dim_y            = -1;
      devInfo.max_grid_dim_z            = -1;
      devInfo.major                     = -1;
      devInfo.minor                     = -1;
    }
    else if ( dev_type_info.id == PAPI_DEV_TYPE_ID__NVIDIA_GPU && this->numDevices_ > 0 ) {
      PAPI_gpu_info_u *gpu_info = (PAPI_gpu_info_u *) dev_type_info.dev_info_arr;
      this->is_gpu_ = true;

      devInfo.name                      = gpu_info->nvidia.name;
      devInfo.warp_size                 = gpu_info->nvidia.warp_size;
      devInfo.max_threads_per_block     = gpu_info->gpu_info->nvidia.max_threads_per_block;
      devInfo.max_blocks_per_multi_proc = gpu_info->nvidia.max_blocks_per_multi_proc;
      devInfo.max_shmem_per_multi_proc  = gpu_info->nvidia.max_shmmem_per_block;
      devInfo.max_block_dim_x           = gpu_info->nvidia.max_block_dim_x;
      devInfo.max_block_dim_y           = gpu_info->nvidia.max_block_dim_y;
      devInfo.max_block_dim_z           = gpu_info->nvidia.max_block_dim_z;
      devInfo.max_grid_dim_x            = gpu_info->nvidia.max_grid_dim_x;
      devInfo.max_grid_dim_y            = gpu_info->nvidia.max_grid_dim_y;
      devInfo.max_grid_dim_z            = gpu_info->nvidia.max_grid_dim_z;
      devInfo.major                     = gpu_info->nvidia.major;
      devInfo.minor                     = gpu_info->nvidia.minor;
    }
    else if ( dev_type_info.id == PAPI_DEV_TYPE_ID__AMD_GPU && this->numDevices_ > 0 ) {
      PAPI_gpu_info_u *gpu_info = (PAPI_gpu_info_u *) dev_type_info.dev_info_arr;
      this->is_gpu_ = true;

      devInfo.name                      = gpu_info->amd.name;
      devInfo.warp_size                 = gpu_info->amd.warp_size;
      devInfo.max_threads_per_block     = gpu_info->gpu_info->amd.max_threads_per_block;
      devInfo.max_blocks_per_multi_proc = gpu_info->amd.max_blocks_per_multi_proc;
      devInfo.max_shmem_per_multi_proc  = gpu_info->amd.max_shmmem_per_block;
      devInfo.max_block_dim_x           = gpu_info->amd.max_block_dim_x;
      devInfo.max_block_dim_y           = gpu_info->amd.max_block_dim_y;
      devInfo.max_block_dim_z           = gpu_info->amd.max_block_dim_z;
      devInfo.max_grid_dim_x            = gpu_info->amd.max_grid_dim_x;
      devInfo.max_grid_dim_y            = gpu_info->amd.max_grid_dim_y;
      devInfo.max_grid_dim_z            = gpu_info->amd.max_grid_dim_z;
      devInfo.major                     = gpu_info->amd.major;
      devInfo.minor                     = gpu_info->amd.minor;
    }
  }

} // namespace pz
