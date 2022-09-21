#pragma once

/** @file commpp.ipp
 *
 *  This file contains the inline definitions for the CommPP class. It is not
 *  meant for inclusion in any other file besides commpp.hpp
 */

namespace parallelzone::mpi_helpers {

template<typename T>
gather_return_t<std::decay_t<T>> CommPP::gather(T&& input,
                                                size_type root) const {
    return gather_t_(std::forward<T>(input), root);
}

template<typename T>
std::decay_t<T> CommPP::gather(T&& input) const {
    return *gather_t_(std::forward<T>(input));
}

template<typename T>
gather_return_t<std::decay_t<T>> CommPP::gather_t_(T&& input,
                                                   opt_root_t root) const {
    using clean_type  = std::decay_t<T>;
    using return_type = gather_return_t<clean_type>;
    using value_type  = typename return_type::value_type;

    const bool am_i_root = root.has_value() ? me() == *root : true;

    if constexpr(needs_serialized_v<T>) {
        // Do gather in binary
        auto binary    = make_binary_buffer(std::forward<T>(input));
        auto binary_rv = gather_(binary, root);

        // Early out if not root
        return_type rv;
        if(!binary_rv.has_value()) return rv;

        // We got back a std::vector<std::byte> which contains this->size()
        // instances of type clean_type, the serialized form of each clean_type
        // object has size binary_rv->size() / this->size()
        const auto& buffer   = *binary_rv;
        auto serialized_size = buffer.size() / size();

        value_type vec(size());
        for(size_type i = 0, j = 0; i < buffer.size();
            i += serialized_size, ++j) {
            const_binary_reference view(binary_rv->data() + i, serialized_size);
            vec[j] = std::move(from_binary_view<clean_type>(view));
        }
        rv.emplace(std::move(vec));
        return rv;
    } else {
        const_binary_reference input_binary(input.data(), input.size());

        // Make output buffer, but only allocate on root
        value_type output;
        if(am_i_root) value_type(input.size() * size()).swap(output);
        binary_reference output_binary(output.data(), output.size());

        gather_(input_binary, output_binary, root);

        return_type rv;
        if(am_i_root) rv.emplace(std::move(output));
        return rv;
    }
}

} // namespace parallelzone::mpi_helpers
