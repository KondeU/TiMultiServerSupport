#pragma once

#include "Serializer.hpp"

namespace ti {
namespace serializer {

class SyncDataSerializer : public Serializer {
public:
    struct SyncDataBase {
        virtual void Serializer(std::stringstream& ss) = 0;
        virtual void Deserializer(std::stringstream& ss) = 0;
    };
    using SyncDataBasePtr = SyncDataBase*;

    template <typename Data>
    struct SyncData : public SyncDataBase {
        void Serializer(std::stringstream& ss) override
        {
            OutputArchive archive(ss);
            archive(data);
        }

        void Deserializer(std::stringstream& ss) override
        {
            InputArchive archive(ss);
            archive(data);
        }

        Data data;
    };

    template <typename Hash>
    void Serializer(std::string& data, const uint32_t& msgId,
        const Hash& hash, const SyncDataBase& syncDataBaseRef)
    {
        {
            OutputArchive archive;
            archive(msgId, hash);
        }
        syncDataBaseRef.Serializer(ss);
        data = ss.str();
        ss.str("");
    }

    template <typename Hash>
    void Deserializer(const std::string& data, uint32_t& msgId,
        Hash& hash, const std::function<SyncDataBasePtr(Hash)>& dp)
    {
        ss.str(data);
        {
            InputArchive archive;
            archive(msgId, hash);
        }
        SyncDataBasePtr syncDataBasePtr = dp(hash);
        if (syncDataBasePtr != nullptr) {
            syncDataBasePtr->Deserializer(ss);
        }
        ss.str("");
    }

private:
    std::stringstream ss;
};

}
}
