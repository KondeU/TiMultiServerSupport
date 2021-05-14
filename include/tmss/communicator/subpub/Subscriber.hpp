#pragma once

#include <zmq.hpp>

namespace ti {
namespace communicator {

class Subscriber {
private:
    friend class Communicator;

    explicit Subscriber(zmq::context_t& context)
        : context(context), socket(context, zmq::socket_type::sub)
    {
    }

    zmq::context_t& context;
    zmq::socket_t socket;
};
using SubscriberInst = Subscriber*;

}
}
