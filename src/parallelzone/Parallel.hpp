#ifndef _PARALLEL_HPP_
#define _PARALLEL_HPP_

#include <thread>

// Macros used to eliminate excess output on large parallel runs
//
//   Note, make sure that ParallelZone::MPI::Init (or
//   ParallelZone::MPI::Init_thread) is called before using isProc0_macro.
//
#define isProc0_macro                               \
    (ParallelZone::Parallel::getMPIRank() == 0) &&  \
      (ParallelZone::Parallel::getMainThreadID() == \
       std::this_thread::get_id())

#define proc0cout \
    if(isProc0_macro) std::cout
#define proc0cerr \
    if(isProc0_macro) std::cerr

#define MAX_THREADS 64

namespace ParallelZone {

class ProcessorGroup;

class Parallel {
public:
    enum Circumstances { NormalShutdown, Abort };

    //////////
    // Initializes MPI if necessary.
    static void initializeManager(int& argc, char**& arg);

    //////////
    // Check to see whether initializeManager has been called
    static bool isInitialized();

    //////////
    // Shuts down and finalizes the MPI runtime in a safe manner
    static void finalizeManager(Circumstances cirumstances = NormalShutdown);

    //////////
    // Returns the root context ProcessorGroup
    static ProcessorGroup* getRootProcessorGroup();

    //////////
    // Returns the MPI Rank of this process.  If this is not running under MPI,
    // than 0 is returned.  Rank value is set after call to initializeManager();
    static int getMPIRank();

    //////////
    // Returns the size of MPI_Comm
    static int getMPISize();

    //////////
    // Returns true if this process is using MPI
    static bool usingMPI();

    //////////
    // Returns true if this process is to use an accelerator or co-processor
    // (e.g. GPU, MIC, etc), false otherwise
    static bool usingDevice();

    //////////
    // Sets whether or not to use available accelerators or co-processors (e.g.
    // GPU, MIC, etc)
    static void setUsingDevice(bool state);

    //////////
    // Returns the number of threads that a processing element is allowed to use
    // to compute its tasks.
    static int getNumThreads();

    //////////
    // Returns the number of thread partitions that a processing element is
    // allowed to use to compute its tasks.
    static int getNumPartitions();

    //////////
    // Returns the number of threads per partition.
    static int getThreadsPerPartition();

    //////////
    // Returns the ID of the main thread, via std::this_thread::get_id()
    static std::thread::id getMainThreadID();

    //////////
    // Sets the number of task runner threads to the value specified
    static void setNumThreads(int num);

    //////////
    // Sets the number of task runner OMP thread partitions to the value
    // specified
    static void setNumPartitions(int num);

    //////////
    // Sets the number of threads per OMP partition
    static void setThreadsPerPartition(int num);

    //////////
    // Passes the specified exit code to std::exit()
    static void exitAll(int code);

private:
    Parallel();
    ~Parallel();

    Parallel(const Parallel&) = delete;
    Parallel& operator=(const Parallel&) = delete;
    Parallel(Parallel&&)                 = delete;
    Parallel& operator=(Parallel&&) = delete;

    static bool s_initialized;
    static bool s_using_device;
    static int s_num_threads;
    static int s_num_partitions;
    static int s_threads_per_partition;
    static int s_world_rank;
    static int s_world_size;
    static std::thread::id s_main_thread_id;
    static ProcessorGroup* s_root_context;
};

} // End namespace ParallelZone
#endif // end _PARALLEL_HPP_
