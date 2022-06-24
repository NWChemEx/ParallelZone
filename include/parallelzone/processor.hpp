#pragma once
#include "parallelzone/pz_cpu.hpp"
#include "parallelzone/pz_gpu.hpp"

namespace parallelzone {
using variant_output = std::variant<parallelzone::BinaryOutputArchive*,
                                    parallelzone::PortableBinaryOutputArchive*,
                                    parallelzone::JSONOutputArchive*,
                                    parallelzone::XMLOutputArchive*>;
using variant_input =
  std::variant<parallelzone::BinaryInputArchive*,
               parallelzone::PortableBinaryInputArchive*,
               parallelzone::JSONInputArchive*, parallelzone::XMLInputArchive*>;

/** @brief ArchiveWrapper class enables serialization and deserialization of
 * objects using various types of archives supported by MADNESS and Cereal.
 *
 *  This class wraps MADNESS/Cereal API for different output archive types.
 * Required for serialization of Any.
 */
template<typename DeviceType>
class Processor {
public:
    template<typename Archive>
    Processor(Archive& ar) : m_ar_(&ar) {}

    template<typename T>
    Processor& operator()(T&& obj) {
        // MADNESS archive doesn't support `()`, but it supports `&` like Boost
        // archive.
        std::visit([&](auto&& ar) { (*ar) & std::forward<T>(obj); }, m_ar_);
        return *this;
    }

    template<typename T>
    Processor& operator<<(T&& obj) {
        std::visit([&](auto&& ar) { (*ar) << std::forward<T>(obj); }, m_ar_);
        return *this;
    }

    template<typename T>
    Processor& operator>>(T&& obj) {
        std::visit([&](auto&& ar) { (*ar) >> std::forward<T>(obj); }, m_ar_);
        return *this;
    }

private:
    DeviceType m_ar_;
};

using CPUProc = Processor<variant_output>;
using GPUProc = Processor<variant_input>;
} // namespace parallelzone
