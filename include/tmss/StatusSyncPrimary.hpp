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

    template <typename T>
    void BindData(DataSet<T>& data, const std::string& category = "default")
    {
        data.category = category;
        data.setChange = [this, &data]()
        {
            std::string serialized;
            serializer.Serialize(serialized,
                networkPackageCounter, data.HashCode(), data);
            if (!Communicator().IsInstInvalid(publisher)) {
                publisher->FanOut(data.category, serialized);
            }
            networkPackageCounter++;
        };
    }

private:
    serializer::SyncDataSerializer serializer;
    communicator::PublisherInst publisher;

    uint32_t networkPackageCounter = 0;
};

}
