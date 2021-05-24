#pragma once

#include "MultiServerSupportBase.hpp"
#include "serializer/FunctionSerializer.hpp"
#include "RpcBasicTemplate.hpp"

namespace ti {

class RpcAsyncBroadcast : public MultiServerSupportBase {
public:
    enum class Role {
        None,
        Server,
        Client
    };

    RpcAsyncBroadcast()
    {
        Communicator().ResetInstInvalid(responder);
        Communicator().ResetInstInvalid(requester);
        Communicator().ResetInstInvalid(publisher);
        Communicator().ResetInstInvalid(subscriber);
    }

private:
    std::unordered_map<std::string, std::function<
        void(const std::string&, std::string&)>> rpcs;
    std::function<void(const std::string&, std::string&)> process;

    serializer::FunctionSerializer serializer;

    communicator::ResponderInst  responder;  // Server
    communicator::RequesterInst  requester;  // Client
    communicator::PublisherInst  publisher;  // Server
    communicator::SubscriberInst subscriber; // Client

    Role role = Role::None;
};

}
