#pragma once

#include <zmq.hpp>
#include "../CommunicationCode.hpp"

namespace ti {
namespace communicator {

class Requester {
public:
    bool Init(const std::string& addr)
    {
        // addr: server address string:
        //      x.x.x.x:x means ip:port
        // Requester is Req/Rep model's Client.
        socket.connect("tcp://" + addr);
        return true;
    }

    int SetTimeout(int ms)
    {
        socket.set(zmq::sockopt::rcvtimeo, ms);
        return socket.get(zmq::sockopt::rcvtimeo);
    }

    CommunicationCode Request(const std::string& request, std::string& respond)
    {
        zmq::message_t reqMsg(request.data(), request.size());
        socket.send(reqMsg, zmq::send_flags::none);

        zmq::message_t respMsg;
        socket.recv(respMsg);

        if (respMsg.size() <= 0) {
            return CommunicationCode::ReceiveTimeout;
        }

        respond = respond.replace(respond.begin(), respond.end(),
            static_cast<char*>(respMsg.data()), respMsg.size());

        return CommunicationCode::Success;
    }

    virtual ~Requester()
    {
        socket.close();
    }

private:
    friend class Communicator;

    explicit Requester(zmq::context_t& context)
        : context(context), socket(context, ZMQ_REQ)
    {
    }

    zmq::context_t& context;
    zmq::socket_t socket;
};
using RequesterInst = Requester*;

}
}
