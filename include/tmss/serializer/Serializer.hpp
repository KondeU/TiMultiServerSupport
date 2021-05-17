#pragma once

#include <cereal/cereal.hpp>
#include <cereal/archives/portable_binary.hpp>
#include "SupportType.hpp" // headers of <cereal/types/*>

namespace ti {
namespace serializer {

class Serializer {
protected:
    using OutputArchive = cereal::PortableBinaryOutputArchive;
    using InputArchive = cereal::PortableBinaryInputArchive;
};

}
}
