#include <parallelzone/resourceset.hpp>

namespace parallelzone {

Resourceset::Resourceset() {
    PAPI_library_init(PAPI_VER_CURRENT);
    papi_info = PAPI_get_hardware_info();

    for(int dev_type_id = 0; dev_type_id < PAPI_DEV_TYPE_ID__MAX_NUM;
        dev_type_id++) {
        if(PAPI_IS_DEV_CPU(papi_info, dev_type_id)) {
            for(int dev_id = 0; dev_id < PAPI_DEV_COUNT(dev_type_id);
                dev_id++) {
                PAPI_cpu_info_t* dev_info =
                  PAPI_CPU_INFO_STRUCT(papi_info, dev_type_id, dev_id);
                printf("Id                                    : %d\n", dev_id);
                printf("Name                                  : %s\n",
                       dev_info->name);
                printf("CPUID                                 : "
                       "Family/Model/Stepping %d/%d/%d 0x%02x/0x%02x/0x%02x\n",
                       dev_info->cpuid_family, dev_info->cpuid_model,
                       dev_info->cpuid_stepping, dev_info->cpuid_family,
                       dev_info->cpuid_model, dev_info->cpuid_stepping);
                printf("Sockets                               : %d\n",
                       dev_info->sockets);
                printf("Numa regions                          : %d\n",
                       dev_info->numas);
                printf("Cores per socket                      : %d\n",
                       dev_info->cores);
                printf(
                  "Cores per NUMA region                 : %d\n",
                  (dev_info->threads * dev_info->cores * dev_info->sockets) /
                    dev_info->numas);
                printf("SMT threads per core                  : %d\n",
                       dev_info->threads);

                for(int k = 0; k < dev_info->numas; ++k) {
                    printf("Numa Node %d Memory                    : %d KB\n",
                           k, dev_info->numa_memory[k]);
                }

                int threads =
                  dev_info->threads * dev_info->cores * dev_info->sockets;
                int numas          = dev_info->numas;
                int* numa_affinity = dev_info->numa_affinity;
                // int *num_threads_per_numa =
                // get_num_threads_per_numa(numa_affinity, numas, threads); int
                // **numa_threads = get_threads_per_numa(numa_affinity,
                // num_threads_per_numa, numas, threads);

                // for (k = 0; k < numas; ++k) {
                //     printf( "Numa Node %d Threads                   : ", k );
                //     int l;
                //     for (l = 0; l < num_threads_per_numa[k]; ++l) {
                //         printf( "%d ", numa_threads[k][l] );
                //     }
                //     printf( "\n" );
                // }
                // free_numa_threads(numa_threads, numas);
                // free(num_threads_per_numa);
                printf("\n");
            }
        }

        if(PAPI_IS_DEV_GPU(NVIDIA, papi_info, dev_type_id)) {
            for(int dev_id = 0; dev_id < PAPI_DEV_COUNT(dev_type_id);
                dev_id++) {
                PAPI_gpu_info_u* dev_info =
                  PAPI_GPU_INFO_STRUCT(papi_info, dev_type_id, dev_id);
                printf("Id                                    : %d\n", dev_id);
                printf("Name                                  : %s\n",
                       dev_info->nvidia.name);
                printf("Warp size                             : %d\n",
                       dev_info->nvidia.warp_size);
                printf("Max threads per block                 : %d\n",
                       dev_info->nvidia.max_threads_per_block);
                printf("Max blocks per multiprocessor         : %d\n",
                       dev_info->nvidia.max_blocks_per_multi_proc);
                printf("Max shared memory per block           : %d\n",
                       dev_info->nvidia.max_shmmem_per_block);
                printf("Max shared memory per multiprocessor  : %d\n",
                       dev_info->nvidia.max_shmmem_per_multi_proc);
                printf("Max block dim x                       : %d\n",
                       dev_info->nvidia.max_block_dim_x);
                printf("Max block dim y                       : %d\n",
                       dev_info->nvidia.max_block_dim_y);
                printf("Max block dim z                       : %d\n",
                       dev_info->nvidia.max_block_dim_z);
                printf("Max grid dim x                        : %d\n",
                       dev_info->nvidia.max_grid_dim_x);
                printf("Max grid dim y                        : %d\n",
                       dev_info->nvidia.max_grid_dim_y);
                printf("Max grid dim z                        : %d\n",
                       dev_info->nvidia.max_grid_dim_z);
                printf("Multiprocessor count                  : %d\n",
                       dev_info->nvidia.multi_processor_count);
                printf("Multiple kernel per context           : %s\n",
                       dev_info->nvidia.multi_kernel_per_ctx ? "yes" : "no");
                printf("Can map host memory                   : %s\n",
                       dev_info->nvidia.can_map_host_mem ? "yes" : "no");
                printf("Can overlap compute and data transfer : %s\n",
                       dev_info->nvidia.can_overlap_comp_and_data_xfer ? "yes" :
                                                                         "no");
                printf("Has unified addressing                : %s\n",
                       dev_info->nvidia.unified_addressing ? "yes" : "no");
                printf("Has managed memory                    : %s\n",
                       dev_info->nvidia.managed_memory ? "yes" : "no");
                printf("Compute capability                    : %d.%d\n",
                       dev_info->nvidia.major, dev_info->nvidia.minor);
                printf("\n");
            }
        }
    }

    // PAPI_library_init(PAPI_VER_CURRENT);

    // papi_info = *PAPI_get_hardware_info();
    // this->papi_info = std::cref( PAPI_get_hardware_info() );
    // papi_info( *PAPI_get_hardware_info() )
}

Resourceset::~Resourceset() { PAPI_shutdown(); }

} // namespace parallelzone
