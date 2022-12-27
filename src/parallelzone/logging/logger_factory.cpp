#include "detail_/spdlog/stdout.hpp"
#include <parallelzone/logging/logger_factory.hpp>

namespace parallelzone {

LoggerFactory::logger_type LoggerFactory::default_global_logger(
  mpi_rank_type rank) {
    Logger log;
    if(rank == 0) {
        auto pimpl = parallelzone::detail_::StdoutSpdlog("Rank 0");
        Logger(pimpl.clone()).swap(log);
    }
    return log;
}

} // namespace parallelzone
