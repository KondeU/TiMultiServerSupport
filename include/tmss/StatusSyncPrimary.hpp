#pragma once

#include "MultiServerSupportBase.hpp"
#include "StatusSyncDataSet.hpp"

namespace ti {

class StatusSyncPrimary : public MultiServerSupportBase {
public:
    StatusSyncPrimary()
    {
        Communicator().ResetInstInvalid(publisher);
    }

    bool StartHosting(const std::string& selfIp, int selfPort)
    {
        if (!Communicator().IsInstInvalid(publisher)) {
            return false;
        }

        publisher = Communicator().CreatePublisher(
            selfIp + ":" + std::to_string(selfPort));
        if (Communicator().IsInstInvalid(publisher)) {
            return false;
        }

        return true;
    }

    bool StopHosting()
    {
        if (Communicator().IsInstInvalid(publisher)) {
            return false;
        }

        Communicator().DestroyInstance(Communicator().MakeInstValue(publisher));
        Communicator().ResetInstInvalid(publisher);

        return true;
    }

    template <typename T>
    bool BindData(DataSet<T>& data, const std::string& category = "default")
    {
        if (data.setChange || data.onChanged) {
            return false;
        }

        data.category = category;
        data.setChange = [this, &data]()
        {
            std::string serialized;
            serializer.Serialize(serialized, *data.networkCounter, data.HashCode(), data);
            if (!Communicator().IsInstInvalid(publisher)) {
                publisher->FanOut(data.category, serialized);
            }
            (*data.networkCounter)++;
            serializer.Serialize(serialized, data.HashCode(), data);
            lastValueCachingSwap[data.valueCache] = serialized;
        };
        data.networkCounter = &networkPackageCounter[data.category];
        data.valueCache = &lastValueCache[data.category][data.HashCode()];

        return true;
    }

    void FlashLastValueCache()
    {
        for (auto& eachCache : lastValueCachingSwap) {
            *eachCache.first = eachCache.second;
        }
        lastValueCachingSwap.clear();
    }

private:
    serializer::SyncDataSerializer serializer;
    communicator::PublisherInst publisher;

    RpcProcessResponse rpcResponder;

    // networkPackageCounter is used to count the number of message.
    // If the number (msgId) is discontinuous, it means that there is
    // a packet loss at least, so secondary should request a LVC again.
    std::unordered_map<std::string, uint32_t> networkPackageCounter;

    // LVC, Memory layout:
    // lastValueCachingSwap: <pointer to cache, cache> - snapshot cache
    // lastValueCache:       <category, <hash, cache>> - snapshot
    std::unordered_map<std::string*, std::string> lastValueCachingSwap;
    std::unordered_map<std::string, std::unordered_map<DataHash, std::string>> lastValueCache;

    // TODO: mutex of {each lastValueCachingSwap first}?
};

}
