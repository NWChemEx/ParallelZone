#include "Parallel.hpp"

#include "Exception.hpp"
// #include <Malloc/Allocator.h>
// #include <Parallel/ProcessorGroup.h>
#include "ParallelZoneMPI.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <thread>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace ParallelZone;

#define THREADED_MPI_AVAILABLE

#if defined(__digital__) || defined(_AIX)
#undef THREADED_MPI_AVAILABLE
#endif

bool Parallel::s_initialized               = false;
bool Parallel::s_using_device              = false;
int Parallel::s_num_threads                = -1;
int Parallel::s_num_partitions             = -1;
int Parallel::s_threads_per_partition      = -1;
int Parallel::s_world_rank                 = -1;
int Parallel::s_world_size                 = -1;
std::thread::id Parallel::s_main_thread_id = std::this_thread::get_id();
ProcessorGroup* Parallel::s_root_context   = nullptr;

namespace ParallelZone {

// While worldComm_ should be declared in Parallel.hpp, I would need to
// #include mpi.h, which then makes about everything in ParallelZone
// depend on mpi.h, so I'm just going to create it here.

static MPI_Comm worldComm_ = MPI_Comm(-1);

} // namespace ParallelZone

//_____________________________________________________________________________
//
static void mpiErr(char* what, int errorcode) {
    // Simple error handling for now...
    int resultlen = -1;
    char string_name[MPI_MAX_ERROR_STRING];

    ParallelZone::MPI::Error_string(errorcode, string_name, &resultlen);
    std::cerr << "MPI Error in " << what << ": " << string_name << '\n';

    std::exit(1);
}

//_____________________________________________________________________________
//
bool Parallel::usingMPI() {
    // TODO: Remove this method once all prior usage of Parallel::usingMPI() is
    // gone We now assume this to be an invariant for ParallelZone, and hence
    // this is always true.
    return true;
}

//_____________________________________________________________________________
//
bool Parallel::usingDevice() { return s_using_device; }

//_____________________________________________________________________________
//
void Parallel::setUsingDevice(bool state) { s_using_device = state; }

//_____________________________________________________________________________
//
int Parallel::getNumThreads() { return s_num_threads; }

//_____________________________________________________________________________
//
int Parallel::getNumPartitions() { return s_num_partitions; }

//_____________________________________________________________________________
//
int Parallel::getThreadsPerPartition() { return s_threads_per_partition; }

//_____________________________________________________________________________
//
std::thread::id Parallel::getMainThreadID() { return s_main_thread_id; }

//_____________________________________________________________________________
//
void Parallel::setNumThreads(int num) { s_num_threads = num; }

//_____________________________________________________________________________
//
void Parallel::setNumPartitions(int num) { s_num_partitions = num; }

//_____________________________________________________________________________
//
void Parallel::setThreadsPerPartition(int num) {
    s_threads_per_partition = num;
}

//_____________________________________________________________________________
//
bool Parallel::isInitialized() { return s_initialized; }

//_____________________________________________________________________________
//
void Parallel::initializeManager(int& argc, char**& argv) {
    s_initialized = true;

    if(s_world_rank != -1) { // IF ALREADY INITIALIZED, JUST RETURN...
        return;
        // If s_world_rank is not -1, then we have already been initialized..
        // This only happens (I think) if usage() is called (due to bad
        // input parameters (to sus)) and usage() needs to init MPI so that
        // it only displays the usage to the root process.
    }

#ifdef _OPENMP
    s_num_partitions = omp_get_max_threads();
#else
    s_num_partitions = 1;
#endif

    if(s_threads_per_partition <= 0) {
#ifdef _OPENMP
        s_threads_per_partition = omp_get_max_threads() / getNumPartitions();
#else
        s_threads_per_partition = 1;
#endif
    }

#ifdef THREADED_MPI_AVAILABLE
    int provided = -1;
    int required = MPI_THREAD_SINGLE;
    if(s_num_threads > 0 || s_num_partitions > 0) {
        required = MPI_THREAD_MULTIPLE;
    } else {
        required = MPI_THREAD_SINGLE;
    }
#endif

    int status;

#ifdef THREADED_MPI_AVAILABLE
    if((status = ParallelZone::MPI::Init_thread(&argc, &argv, required,
                                                &provided)) != MPI_SUCCESS) {
#else
    if((status = ParallelZone::MPI::Init(&argc, &argv)) != MPI_SUCCESS) {
#endif
        mpiErr(const_cast<char*>("ParallelZone::MPI::Init"), status);
    }

#ifdef THREADED_MPI_AVAILABLE
    if(provided < required) {
        std::cerr << "Provided MPI parallel support of " << provided
                  << " is not enough for the required level of " << required
                  << "\n"
                  << "To use multi-threaded scheduler, your MPI implementation "
                     "needs to support MPI_THREAD_MULTIPLE (level-3)"
                  << std::endl;
        throw ParallelZone::pz_exception("Bad MPI level", __FILE__, __LINE__);
    }
#endif

    ParallelZone::worldComm_ = MPI_COMM_WORLD;
    if((status = ParallelZone::MPI::Comm_size(ParallelZone::worldComm_,
                                              &s_world_size)) != MPI_SUCCESS) {
        mpiErr(const_cast<char*>("ParallelZone::MPI::Comm_size"), status);
    }

    if((status = ParallelZone::MPI::Comm_rank(ParallelZone::worldComm_,
                                              &s_world_rank)) != MPI_SUCCESS) {
        mpiErr(const_cast<char*>("ParallelZone::MPI::Comm_rank"), status);
    }

    s_root_context =
      scinew ProcessorGroup(nullptr, ParallelZone::worldComm_, s_world_rank,
                            s_world_size, s_num_threads);

    if(s_root_context->myRank() == 0) {
        std::string plural =
          (s_root_context->nRanks() > 1) ? "processes" : "process";
        std::cout << "Parallel: " << s_root_context->nRanks() << " MPI "
                  << plural << " (using MPI)\n";

#ifdef THREADED_MPI_AVAILABLE
        if(s_num_threads > 0) {
            std::cout << "Parallel: " << s_num_threads
                      << " threads per MPI process\n";
        }
        std::cout << "Parallel: MPI Level Required: " << required
                  << ", provided: " << provided << "\n";
#endif
    }
    //    ParallelZone::MPI::Errhandler_set(ParallelZone::worldComm_,
    //    MPI_ERRORS_RETURN);
}

//_____________________________________________________________________________
//
int Parallel::getMPIRank() {
    if(s_world_rank == -1) {
        // Can't throw an exception here because it won't get trapped
        // properly because 'getMPIRank()' is called in the exception
        // handler...
        std::cout << "ERROR:\n";
        std::cout
          << "ERROR: getMPIRank() called before initializeManager()...\n";
        std::cout << "ERROR:\n";
        exitAll(1);
    }
    return s_world_rank;
}

//_____________________________________________________________________________
//
int Parallel::getMPISize() { return s_world_size; }

//_____________________________________________________________________________
//
void Parallel::finalizeManager(
  Circumstances circumstances /* = NormalShutdown */) {
    static bool finalized = false;

    if(finalized) {
        // Due to convoluted logic, signal, and exception handling,
        // finalizeManager() can be easily/mistakenly called multiple
        // times.  This catches that case and returns harmlessly.
        //
        // (One example of this occurs when ParallelZone::MPI::Abort causes a
        // SIG_TERM to be thrown, which is caught by ParallelZone's exit
        // handler, which in turn calls finalizeManager.)
        return;
    }

    finalized = true;

    // s_world_rank is not reset here as even after finalizeManager,
    // some things need to know their rank...

    // Only finalize if MPI was initialized.
    if(s_initialized == false) {
        throw ParallelZone::pz_exception(
          "Trying to finalize without having MPI initialized", __FILE__,
          __LINE__);
    }

    if(circumstances == Abort) {
        int errorcode = 1;
        if(s_world_rank == 0) {
            std::cout << "FinalizeManager() called... Calling "
                         "ParallelZone::MPI::Abort on rank "
                      << s_world_rank << ".\n";
        }
        std::cerr.flush();
        std::cout.flush();

        double seconds = 1.0;

        struct timespec ts;
        ts.tv_sec  = (int)seconds;
        ts.tv_nsec = (int)(1.e9 * (seconds - ts.tv_sec));

        nanosleep(&ts, &ts);

        ParallelZone::MPI::Abort(ParallelZone::worldComm_, errorcode);
    } else {
        int status;
        if((status = ParallelZone::MPI::Finalize()) != MPI_SUCCESS) {
            mpiErr(const_cast<char*>("ParallelZone::MPI::Finalize"), status);
        }
    }

    if(s_root_context != nullptr) {
        delete s_root_context;
        s_root_context = nullptr;
    }
}

//_____________________________________________________________________________
//
ProcessorGroup* Parallel::getRootProcessorGroup() {
    if(s_root_context == nullptr) {
        throw ParallelZone::pz_exception("Parallel not initialized", __FILE__,
                                         __LINE__);
    }

    return s_root_context;
}

//_____________________________________________________________________________
//
void Parallel::exitAll(int code) { std::exit(code); }
