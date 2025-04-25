#include <parallelzone/hardware/cpu/cpu.hpp>

namespace parallelzone::hardware {

typename CPU::profile_return_type CPU::profile_it_(task_type&& task) const {
    profile_information i;
    const auto t1 = std::chrono::high_resolution_clock::now();
    auto result   = task();
    const auto t2 = std::chrono::high_resolution_clock::now();
    i.wall_time   = (t2 - t1);
    return std::make_pair(std::move(result), std::move(i));
}

} // namespace parallelzone::hardware