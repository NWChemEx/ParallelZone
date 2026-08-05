/*
 * Copyright 2025 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <chrono>
#include <parallelzone/task/task_wrapper.hpp>

namespace parallelzone::hardware {

/** @brief Aggregates known profiling information.
 *
 *  The class is primarily intended to ensure that `profile_it` can maintain a
 *  consistent API as new profiling information is added, i.e.,
 *  `ProfileInformation` is intended to be an opaque type whose content can
 *   grow over time.
 */
struct ProfileInformation {
    /// Type used to measure time durations
    using duration = std::chrono::high_resolution_clock::duration;

    /// Long the function ran for
    duration wall_time;
};

/** @brief Class representing a central processing unit (CPU).
 *
 *  This class is intended to be a runtime interface for interacting with the
 *  CPU. Right now it can be used to profile a task run on a CPU.
 */
class CPU {
private:
    /// Type used internally for wrapping tasks
    using task_type = task::TaskWrapper;

    /// Type used internally for returning type-erased results
    using result_type = typename task_type::type_erased_return_type;

public:
    /// Type containing profiling information
    using profile_information = ProfileInformation;

    /** @brief Profiles a function.
     *
     *  @tparam FxnType The type of the callable to profile.
     *  @tparam Args The types of the arguments to forward to the callable.
     *
     *  @param[in] fxn The callable to run.
     *  @param[in] args The arguments to forward to the callable.
     *
     *  @return A pair whose first element is the return of your callable and
     *          the second element is the profiling information obtained from
     *          running the callable. If your function does not have a return
     *          only the profiling info is returned.
     *
     *  @throw ??? Throws if the callable throws. Same throw guarantee.
     */
    template<typename FxnType, typename... Args>
    auto profile_it(FxnType&& fxn, Args&&... args) const {
        auto&& [t, unwrapper] = task::make_task(std::forward<FxnType>(fxn),
                                                std::forward<Args>(args)...);
        auto result           = profile_it_(std::move(t));

        using result_type = decltype(unwrapper(std::move(result.first)));

        if constexpr(std::is_same_v<result_type, void>) {
            return std::move(result.second);
        } else {
            auto unwrapped_result = unwrapper(std::move(result.first));
            return std::make_pair(std::move(unwrapped_result),
                                  std::move(result.second));
        }
    }

private:
    /// How the type-erased profile_it_ method returns its results
    using profile_return_type = std::pair<result_type, profile_information>;

    /// Returns the result of the task and the profiling information obtained
    profile_return_type profile_it_(task_type&& task) const;
};

} // namespace parallelzone::hardware
