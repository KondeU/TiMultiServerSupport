#pragma once

#include <zmq.hpp>

namespace ti {
namespace communicator {

class Publisher {
private:
    friend class Communicator;

    explicit Publisher(zmq::context_t& context)
        : context(context), socket(context, zmq::socket_type::pub)
    {
    }

    zmq::context_t& context;
    zmq::socket_t socket;
};
using PublisherInst = Publisher*;

}
}
