#pragma once

#include "Serializer.hpp"

namespace tirpc {
namespace serializer {

class FunctionSerializer : public Serializer {
public:
    template <class ...Args>
    void Serialize(std::string& data, const std::string& func, const Args& ...args)
    {
        // 1. serialize --> data
        {
            OutputArchive archive(ss);
            archive(func, args...);
        }
        // 2. get serialized data from stream
        data = ss.str();
        // 3. reset stream
        ss.str("");
    }

    template <class ...Args>
    void Deserialize(const std::string& data, std::string& func, Args& ...args)
    {
        // 1. set serialized data to stream
        ss.str(data);
        // 2. data --> deserialize
        {
            InputArchive archive(ss);
            archive(func, args...);
        }
        // 3. reset stream
        ss.str("");
    }

private:
    std::stringstream ss;
};

}
}
