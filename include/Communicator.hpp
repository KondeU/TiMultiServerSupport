#pragma once

#include <memory>
#include <unordered_map>

#include <zmq.hpp>

namespace ti {
namespace communicator {

// Req/Resp model: Requester
class Requester {
public:
    bool Init()
    {
    }

private:
    friend class Communicator;

    explicit Requester(zmq::context_t& context) : context(context)
    {
    }

    zmq::context_t& context;
    zmq::socket_t socket;
};
using RequesterInst = Requester*;

// Req/Resp model: Responder
class Responder {
};
using ResponderInst = Responder*;

// Sub/Pub model: Subscriber
class Subscriber {
};
using SubscriberInst = Subscriber*;

// Sub/Pub model: Publisher
class Publisher {
};
using PublisherInst = Publisher*;

// Communicator is a factory
class Communicator {
public:
    using InstValue = uintptr_t;

    template <typename Inst>
    InstValue MakeInstValue(Inst inst)
    {
        static_assert(
            std::is_same< Inst, RequesterInst  >::value ||
            std::is_same< Inst, ResponderInst  >::value ||
            std::is_same< Inst, SubscriberInst >::value ||
            std::is_same< Inst, PublisherInst  >::value ,
            "Instance invalid.");
        return reinterpret_cast<InstValue>(inst);
    }

    template <typename Inst>
    bool IsInstValid(Inst inst)
    {
        static_assert(
            std::is_same< Inst, RequesterInst  >::value ||
            std::is_same< Inst, ResponderInst  >::value ||
            std::is_same< Inst, SubscriberInst >::value ||
            std::is_same< Inst, PublisherInst  >::value,
            "Instance invalid.");
        return (inst != nullptr);
    }

    RequesterInst CreateRequester()
    {
        auto requester = new Requester(context);
        return requester;
    }

    ResponderInst CreateResponder()
    {
        return nullptr;
    }

    SubscriberInst CreateSubscriber()
    {
        return nullptr;
    }

    PublisherInst CreatePublisher()
    {
        return nullptr;
    }

    bool DestroyInstance(InstValue instValue)
    {
        if (requesters.erase(instValue) > 0) {
            return true;
        }
        if (responders.erase(instValue) > 0) {
            return true;
        }

        if (subscribers.erase(instValue) > 0) {
            return true;
        }
        if (publishers.erase(instValue) > 0) {
            return true;
        }

        return false;
    }

private:
    zmq::context_t context;

    std::unordered_map<InstValue, std::unique_ptr<Requester>> requesters;
    std::unordered_map<InstValue, std::unique_ptr<Responder>> responders;

    std::unordered_map<InstValue, std::unique_ptr<Subscriber>> subscribers;
    std::unordered_map<InstValue, std::unique_ptr<Publisher>> publishers;
};

}
}
