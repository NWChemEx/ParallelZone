#pragma once

/** @file ram.hpp
 *
 *  This file contains inline implemenations of RAM class methods. This file
 *  should only be included from ram.hpp.
 */

namespace parallelzone::hardware {

template<typename T>
typename RAM::gather_return_type<std::decay_t<T>> RAM::gather(T&& input) const {
    // using clean_type = std::decay_t<T>;
    // using mpi_helpers::make_binary_view;
    // auto binary_view = make_binary_view(std::forward<T>(input));

    // auto vec_binary = gather(std::move(binary_view));
    // RAM::gather_return_type<clean_type> rv;

    // if(!vec_binary.has_value()) return rv; // Taken by ranks that aren't root

    // // Only the root of the MPI_gather call gets here
    // const auto& value = vec_binary.value();

    // // For Gather each object must have the same size, so the result contains
    // // n_ranks
    // auto total_bytes = value.size();
    // auto n           = total_bytes / sizeof(clean_type);
    // for(decltype(n) i = 0; i < n; i += n)
    //     rv.emplace_back(std::move(from_binary_view(value[i])));

    // return rv;
}

} // namespace parallelzone::hardware
