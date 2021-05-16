#pragma once

#include <zmq_addon.hpp>

namespace ti {
namespace communicator {

class Subscriber {
public:
    bool Init(const std::string& addr)
    {
        // addr: server address string:
        //       x.x.x.x:x means ip:port
        socket.connect("tcp://" + addr);
        return true;
    }

    void Subscribe(const std::string& envelope)
    {
        socket.set(zmq::sockopt::subscribe, envelope);
    }

    void Unsubscribe(const std::string& envelope)
    {
        socket.set(zmq::sockopt::unsubscribe, envelope);
    }

    int SetTimeout(int ms)
    {
        socket.set(zmq::sockopt::rcvtimeo, ms);
        return socket.get(zmq::sockopt::rcvtimeo);
    }

    CommunicationCode RecvIn(std::string& envelope, std::string& content)
    {
        std::vector<zmq::message_t> messages;
        zmq::recv_multipart(socket, std::back_inserter(messages));
        if (messages.size() != 2) { // envelope + content
            return CommunicationCode::ReceiveTimeout;
        }
        // envelope value is a pure string.
        envelope = envelope.replace(envelope.begin(), envelope.end(),
            static_cast<char*>(messages[0].data()));
        content = content.replace(content.begin(), content.end(),
            static_cast<char*>(messages[1].data()), messages[1].size());
        return CommunicationCode::Success;
    }

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
