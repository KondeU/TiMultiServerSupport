#pragma once

#include "Serializer.hpp"

namespace ti {
namespace serializer {

class SyncDataSerializer : public Serializer {
public:
    struct SyncDataBase {
        virtual void Serialize(std::stringstream& ss) = 0;
        virtual void Deserialize(std::stringstream& ss) = 0;
    protected:
        virtual void OnSerialized() {}
        virtual void OnDeserialized() {}
    };
    using SyncDataBasePtr = SyncDataBase*;

    template <typename Data>
    struct SyncData : public SyncDataBase {
        void Serialize(std::stringstream& ss) override
        {
            {
                OutputArchive archive(ss);
                archive(data);
            }
            OnSerialized();
        }

        void Deserialize(std::stringstream& ss) override
        {
            {
                InputArchive archive(ss);
                archive(data);
            }
            OnDeserialized();
        }

        Data data {};
    };

    template <typename Hash>
    void Serialize(std::string& data, const uint32_t& msgId,
        const Hash& hash, SyncDataBase& syncDataBaseReference)
    {
        {
            OutputArchive archive;
            archive(msgId, hash);
        }
        syncDataBaseReference.Serialize(ss);
        data = ss.str();
        ss.str("");
    }

    template <typename Hash>
    void Deserialize(const std::string& data, uint32_t& msgId,
        Hash& hash, const std::function<SyncDataBasePtr(Hash)>& dp)
    {
        ss.str(data);
        {
            InputArchive archive;
            archive(msgId, hash);
        }
        SyncDataBasePtr syncDataBasePtr = dp(hash);
        if (syncDataBasePtr != nullptr) {
            syncDataBasePtr->Deserialize(ss);
        }
        ss.str("");
    }

private:
    std::stringstream ss;
};

}
}
