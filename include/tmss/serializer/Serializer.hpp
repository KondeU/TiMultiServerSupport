#pragma once

#include <cereal/cereal.hpp>
#include <cereal/archives/portable_binary.hpp>
#include "SupportType.hpp" // headers of <cereal/types/*>

namespace tirpc {
namespace serializer {

class Serializer {
protected:
    using OutputArchive = cereal::PortableBinaryOutputArchive;
    using InputArchive = cereal::PortableBinaryInputArchive;
};

}
}
