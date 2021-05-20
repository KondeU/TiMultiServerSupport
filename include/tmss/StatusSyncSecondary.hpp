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

        return true;
    }

    bool StopListening()
    {
        if (Communicator().IsInstInvalid(subscriber)) {
            return false;
        }

        Communicator().DestroyInstance(Communicator().MakeInstValue(subscriber));
        Communicator().ResetInstInvalid(subscriber);

        return true;
    }

private:
    static constexpr int ReceiveTimeout = 1000;

    serializer::SyncDataSerializer serializer;
    communicator::SubscriberInst subscriber;

    RpcProcessRequest rpcRequester;

    std::unordered_map<void*, std::vector<char>> caches;
    std::unordered_map<std::string, uint32_t> networkPackageCounter;
};

}
