#pragma once

#include <mutex>
#include "Serializer.hpp"

namespace ti {
namespace serializer {

class SyncDataBase {
public:
    uint32_t HashCode() const
    {
        return hash;
    }

    virtual void Serialize() = 0;
    virtual void Deserialize() = 0;

protected:
    SyncDataBase()
    {
        std::lock_guard<std::mutex> locker(mutex);
        hash = hashSeed++; // Two-step operation, should not use atomic only!
    }

    virtual void OnChanged()
    {
    }

private:
    uint32_t hash;

    static std::mutex mutex;
    static uint32_t hashSeed;
};
using SyncDataBasePtr = SyncDataBase*;

template <typename T>
class SyncData : public SyncDataBase {
};

}
}
