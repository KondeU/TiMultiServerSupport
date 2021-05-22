#pragma once

#include <zmq_addon.hpp>

namespace ti {
namespace communicator {

class Subscriber {
public:
    int SetTimeout(int ms)
    {
        socket.set(zmq::sockopt::rcvtimeo, ms);
        return socket.get(zmq::sockopt::rcvtimeo);
    }

    void Subscribe(const std::string& envelope)
    {
        socket.set(zmq::sockopt::subscribe, envelope);
    }

    void Unsubscribe(const std::string& envelope)
    {
        socket.set(zmq::sockopt::unsubscribe, envelope);
    }

    CommunicationCode RecvIn(std::string& envelope, std::string& content)
    {
        std::vector<zmq::message_t> messages;
        (void)zmq::recv_multipart(socket, std::back_inserter(messages));
        if (messages.size() != 2) { // envelope + content
            return CommunicationCode::ReceiveTimeout;
        }
        // envelope value is a pure string.
        envelope = messages[0].to_string();
        content = content.replace(content.begin(), content.end(),
            static_cast<char*>(messages[1].data()), messages[1].size());
        return CommunicationCode::Success;
    }

    virtual ~Subscriber()
    {
        socket.close();
    }

private:
    friend class Communicator;

    explicit Subscriber(zmq::context_t& context)
        : context(context), socket(context, zmq::socket_type::sub)
    {
    }

    bool Init(const std::string& addr)
    {
        // addr: server address string:
        //       x.x.x.x:x means ip:port
        socket.connect("tcp://" + addr);
        return true;
    }

    zmq::context_t& context;
    zmq::socket_t socket;
};
using SubscriberInst = Subscriber*;

}
}
