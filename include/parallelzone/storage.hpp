#pragma once
#include "parallelzone/serialization.hpp"
#include <variant>

namespace parallelzone {
using variant_output = std::variant<parallelzone::BinaryOutputArchive*,
                                    parallelzone::PortableBinaryOutputArchive*,
                                    parallelzone::JSONOutputArchive*,
                                    parallelzone::XMLOutputArchive*>;
using variant_input =
  std::variant<parallelzone::BinaryInputArchive*,
               parallelzone::PortableBinaryInputArchive*,
               parallelzone::JSONInputArchive*, parallelzone::XMLInputArchive*>;

/** @brief Storage class enables serialization and deserialization of
 * objects using various types of archives supported by MADNESS and Cereal.
 *
 *  This class wraps MADNESS/Cereal API for different output archive types.
 * Required for serialization of Any.
 */
template<typename VariantArchive>
class Storage {
public:
    template<typename Archive>
    Storage(Archive& ar) : m_ar_(&ar) {}

    template<typename T>
    Storage& operator()(T&& obj) {
        std::visit([&](auto&& ar) { (*ar) & std::forward<T>(obj); }, m_ar_);
        return *this;
    }

private:
    VariantArchive m_ar_;
};

using Serializer   = Storage<variant_output>;
using Deserializer = Storage<variant_input>;
} // namespace parallelzone
