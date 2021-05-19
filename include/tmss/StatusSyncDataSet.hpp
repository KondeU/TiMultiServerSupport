#pragma once

#include <mutex>
#include "serializer/SyncDataSerializer.hpp"

namespace ti {
namespace dataset {

class StatusSyncDataHash {
public:
    using HashType = uint32_t;

    StatusSyncDataHash()
    {
        std::lock_guard<std::mutex> locker(hashMutex);
        hash = hashSeed++; // Two-step operation, do not use atomic.
    }

    inline HashType HashCode() const
    {
        return hash;
    }

private:
    HashType hash;
    static HashType hashSeed;
    static std::mutex hashMutex;
};

template <typename T>
class StatusSyncDataSet :
    public StatusSyncDataHash,
    public serializer::SyncDataSerializer::SyncData<T> {
public:
    // User Interface: use Get to obtain variable in DataSet.
    const T& Get() const
    {
        return data;
    }

    // User Interface: use Set to modify variable in DataSet.
    // Otherwise the synchronization will do not take effect.
    void Set(const T& t)
    {
        data = t;
        if (setChange) {
            setChange();
        }
    }

    void OnDeserialized() override
    {
        if (onChanged) {
            onChanged();
        }
    }

    // The following variables are set in BindData of
    // StatusSyncPrimary or StatusSyncSecondary function.
    std::string category;
    std::function<void()> setChange;
    std::function<void()> onChanged;
    // For the default unordered_map, the element
    // memory inside is always valid and unchanged.
    uint32_t* networkCounter = nullptr; // It is ugly but works.
    std::string* valueCache = nullptr;  // It is ugly but works too.
    // The implementation here is a little compromised, and is planning to fix it.
};

}

// Warning:
// Only Set and Get functions in the DataSet could used.
// Other functions and variables are used internally.
template <typename T>
using DataSet = dataset::StatusSyncDataSet<T>;
using DataHash = dataset::StatusSyncDataHash::HashType;

}
