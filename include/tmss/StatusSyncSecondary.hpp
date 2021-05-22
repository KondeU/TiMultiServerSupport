#pragma once

#include "MultiServerSupportBase.hpp"
#include "StatusSyncDataSet.hpp"

namespace ti {

class StatusSyncSecondary : public MultiServerSupportBase {
public:
    bool StartListening(const std::string& selfIp, int selfPort)
    {
        if (!Communicator().IsInstInvalid(subscriber)) {
            return false;
        }

        subscriber = Communicator().CreateSubscriber(
            selfIp + ":" + std::to_string(selfPort));
        if (Communicator().IsInstInvalid(subscriber)) {
            return false;
        }

        subscriber->SetTimeout(ReceiveTimeout);
        for (auto& each : networkPackageCounter) {
            subscriber->Subscribe(each.first);
        }

        receivingThreadLoop = true;
        receivingThread = std::thread(std::bind(
            &StatusSyncSecondary::ReceivingThread, this));

        return true;
    }

    bool StopListening()
    {
        if (Communicator().IsInstInvalid(subscriber)) {
            return false;
        }

        receivingThreadLoop = false;
        if (receivingThread.joinable()) {
            receivingThread.join();
        }

        Communicator().DestroyInstance(Communicator().MakeInstValue(subscriber));
        Communicator().ResetInstInvalid(subscriber);

        return true;
    }

    template <typename T>
    bool BindData(DataSet<T>& data, T& raw, const std::string& category = "default")
    {
        if (data.setChange || data.onChanged) {
            return false;
        }

        if (networkPackageCounter[category] == 0) { // New category created
            if (!Communicator().IsInstInvalid(subscriber)) {
                subscriber->Subscribe(category);
            }
        }

        data.category = category;
        data.onChanged = [this, &data, &raw]()
        {
            auto& cache = caches[&raw];
            // FIXME! The following codes:
            // It is a compromise solution. :(
            cache.first.resize(sizeof(T));
            T* cacheRawPointer = reinterpret_cast<T*>(cache.first.data());
            new (cacheRawPointer) T; // *cacheRawPointer will save the raw cache.
            // This solution requests equal sign to do copy action.
            *cacheRawPointer = data.data;
            cache.second = [cacheRawPointer](void* to)
            {
                *reinterpret_cast<T*>(to) = *cacheRawPointer;
                cacheRawPointer->~T();
            };
        };
        lookup[data.HashCode()] = &data;

        return true;
    }

    void SynchronizeCaches()
    {
        for (auto& each : caches) {
            each.second.second(each.first); // FIXME! See BindData function.
        }
        caches.clear();
    }

private:
    static constexpr int ReceiveTimeout = 1000;

    serializer::SyncDataSerializer serializer;
    communicator::SubscriberInst subscriber;

    RpcProcessRequest rpcRequester;

    // networkPackageCounter is used to count the number of message.
    // See also: StatusSyncPrimary.hpp variable: networkPackageCounter
    std::unordered_map<std::string, uint32_t> networkPackageCounter;

    // caches is used to make sure the synchronization
    // between the receive thread and the main thread. --------------------------+
    std::unordered_map<void*, std::pair<std::vector<char>, // function to save   |
        std::function<void(void*)>>> caches; // the copy operation and the ~T(). |
    // --------------------------------------------------------------------------+

    std::unordered_map<DataHash, serializer::SyncDataSerializer::SyncDataBasePtr> lookup;

    bool receivingThreadLoop = false;
    std::thread receivingThread;
    void ReceivingThread()
    {
        while (receivingThreadLoop) {
            // network receiving
            std::string category;
            std::string serialized;
            subscriber->RecvIn(category, serialized);
            // deserialize
            uint32_t msgId;
            DataHash hashCode;
            serializer.Deserialize<DataHash>(serialized, msgId, hashCode,
            [this](DataHash hash)->serializer::SyncDataSerializer::SyncDataBasePtr
            {
                return lookup[hash];
            }); // Deserialize -> OnSerialized -> Updated data
            // check and something others
            uint32_t& crntMsgId = networkPackageCounter[category];
            if ((crntMsgId + 1) != msgId) {
                // TODO: Request last value cache...
            }
            //crntMsgId++;
            crntMsgId = msgId; //(temporary)
        }
    }

    // TODO: mutex of caches?
};

}
