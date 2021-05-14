#pragma once

#include "Serializer.hpp"

namespace ti {
namespace serializer {

class SyncDataSerializer : public Serializer {
// Protocol:
// |----head----|---msgid--|-hash-|-data-|
// | uint8_t[4]   uint32_t   Hash   Data |
public:
    static constexpr size_t HeadCount = 4;

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
    void Serializer(std::string& bin, const std::vector<uint8_t>& head,
        const uint32_t& msgid, const Hash& hash, const SyncDataBase& data)
    {
        // Serialize head
        size_t headCount = std::min(head.size(), HeadCount);
        for (size_t n = 0; n < headCount; n++) {
            ss << head[n];
        }
        size_t paddingCount = HeadCount - headCount;
        for (size_t n = 0; n < paddingCount; n++) {
            ss << uint8_t('\0');
        }
        // Serialize info
        {
            OutputArchive archive;
            archive(msgid, hash);
        }
        // Serialize data
        data.Serializer(ss);
        // Serialized binary
        bin = ss.str();
        // Reset stream
        ss.str("");
    }

    template <typename Hash>
    void Deserializer(const std::string& bin, std::vector<uint8_t>& head,
        uint32_t& msgid, Hash& hash, const std::function<SyncDataBasePtr(Hash)>& dp)
    {
        // Serialized binary
        ss.str(bin);
        // Deserialize head
        head.resize(HeadCount);
        for (size_t n = 0; n < HeadCount; n++) {
            ss >> head[n];
        }
        // Deserialize info
        {
            InputArchive archive;
            archive(msgid, hash);
        }
        // Deserialize data
        SyncDataBasePtr pdata = dp(hash);
        if (pdata != nullptr) {
            pdata->Deserializer(ss);
        }
        // Reset stream
        ss.str("");
    }

private:
    std::stringstream ss;
};

}
}
