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

    enum class CallError {
        Success,
        NetworkTimeout,
        FunctionNotFound,
        FunctionNameMismatch
    };

    RpcAsyncBroadcast()
    {
        Communicator().ResetInstInvalid(responder);
        Communicator().ResetInstInvalid(requester);
        Communicator().ResetInstInvalid(publisher);
        Communicator().ResetInstInvalid(subscriber);

        procRep = // NB: Caution that the life cycle of procRep!
        [this](const std::string& request, std::string& respond)
        {
            ResponseProcess(request, respond);
        };
        procSub = // NB: Caution that the life cycle of procSub!
        [this](const std::string& envelope, const std::string& content)
        {
            SubscribeProcess(envelope, content);
        };
        procSubCb = // NB: Caution that the life cycle of procSubCb!
        [this](bool receivedSuccess)
        {
            if (receivedSuccess) {
                receiveTimeoutCounter = 0;
            } else {
                receiveTimeoutCounter++;
                if (receiveTimeoutCounter > HeartbeatThreshold) {
                    // TODO: Ping-Pong test.
                }
            }
        };
    }

    bool Start(Role node, const std::string& ip,
        int callfunc, int broadcast) // port of ReqRep and PubSub
    {
        if (role != Role::None) {
            return false;
        }
        if (node == Role::None) {
            return false;
        }

        receiveTimeoutCounter = 0;

        role = node;
        addrReqRep = ip + ":" + std::to_string(callfunc);
        addrPubSub = ip + ":" + std::to_string(broadcast);

        switch (node) {
        default:
        case Role::None:
            // It is not possible to run here.
            // Only used to avoid compilation warning.
            // It has been checked that node is not Role::None.
            return false;

        case Role::Server:
            // TODO
            break;

        case Role::Client:
            // TODO
            break;
        }
    }

    bool Stop()
    {
        if (role == Role::None) {
            return false;
        }

        // TODO

        role = Role::None;
        addrReqRep = "";
        addrPubSub = "";
    }

    template <typename Func>
    inline void BindFunc(const std::string& name, Func func)
    {
        rpcs[name] = std::bind(&RpcAsyncBroadcast::CallProxy<Func>,
            this, func, std::placeholders::_1);
    }

    template <typename CFunc, typename CType>
    inline void BindFunc(const std::string& name, CFunc func, CType& impl)
    {
        rpcs[name] = std::bind(&RpcAsyncBroadcast::CallProxy<CFunc, CType>,
            this, func, impl, std::placeholders::_1);
    }

    template <typename ...Args>
    CallError CallFunc(const std::string& name, const Args& ...args)
    {
        rpc::RpcFuncArgsWrapper<typename std::decay<Args>::type...>
            wrapper = std::make_tuple(args...);

        std::string request;
        serializer.Serialize(request, name, wrapper);

        std::string respond;
        switch (requester->Request(request, respond)) {
        case communicator::CommunicationCode::Success:
            break; // Success means the network communication is normal.
        case communicator::CommunicationCode::ReceiveTimeout:
            return CallError::NetworkTimeout;
        }

        std::string retFuncName;
        rpc::RpcReturnCode retReturnCode;
        serializer.Deserialize(respond, retFuncName, retReturnCode);
        // Function name mismatch, may be out-of-order calls occurred.
        if (retFuncName != name) {
            return CallError::FunctionNameMismatch;
        }
        // Only Success or FunctionNotFound is depend by the server execute.
        return retReturnCode;
    }

protected:
    //-- ResponseProcess: Only running on the server.
    void ResponseProcess(const std::string& request, std::string& respond)
    {
        std::string funcName;
        fsRep.Deserialize(request, funcName);
        auto funcIter = rpcs.find(funcName);
        if (funcIter != rpcs.end()) {
            publisher->Publish(funcName, request); // publish to all clients
            fsRep.Serialize(respond, funcName, rpc::RpcReturnCode::Success);
        } else {
            fsRep.Serialize(respond, funcName, rpc::RpcReturnCode::FunctionNotFound);
        }
    } // ResponseProcess: Server(ExecFunc), running in responder thread.

    //-- SubscribeProcess: Only running on the client.
    void SubscribeProcess(const std::string& envelope, const std::string& content)
    {
        // Using envelope to store the function name here,
        // which will cause the data package to become larger,
        // another way is deserialize twice locally(see ResponseProcess).
        auto funcIter = rpcs.find(envelope);
        if (funcIter != rpcs.end()) {
            funcIter->second(content);
        }
    } // SubscribeProcess: Client, running in subscriber thread.

    template <typename Func>
    inline void CallProxy(Func func, const std::string& data)
    {
        CallProxyImpl(func, data);
    }

    template <typename CFunc, typename CType>
    inline void CallProxy(CFunc func, CType& impl, const std::string& data)
    {
        CallProxyImpl(func, impl, data);
    }

    template <class ...Args>
    void CallProxyImpl(void(*func)(Args...), const std::string& data)
    {
        CallProxyImpl(std::function<void(Args...)>(func), data);
    }

    template <typename CImpl, class ...Args>
    void CallProxyImpl(void(CImpl::*func)(Args...), CImpl& impl, const std::string& data)
    {
        auto proxy = [&func, &impl](Args ...args) {
            (impl.*func)(args...);
        };
        CallProxyImpl(std::function<void(Args...)>(proxy), data);
    }

    template <class ...Args>
    void CallProxyImpl(std::function<void(Args...)> func, const std::string& data)
    {
        std::string funcName;
        rpc::RpcFuncArgsWrapper<typename std::decay<Args>::type...> funcArgs;
        serializer.Deserialize(data, funcName, funcArgs);
        CallInvoke(func, funcArgs);
    }

    template <typename Func, typename ArgsTuple>
    void CallInvoke(Func&& func, ArgsTuple&& argsTuple)
    {
        constexpr auto Size = std::tuple_size<
            typename std::decay<ArgsTuple>::type>::value;
        CallInvokeImpl(std::forward<Func>(func),
            std::forward<ArgsTuple>(argsTuple),
            std::make_index_sequence<Size>{});
    }

    template <typename Func, typename ArgsTuple, std::size_t ...Index>
    void CallInvokeImpl(Func&& func, ArgsTuple&& argsTuple,
        std::index_sequence<Index...>)
    {
        func(std::get<Index>(std::forward<ArgsTuple>(argsTuple))...);
    }

private:
    static constexpr int RpcTimeout = 10; // 10ms
    // If the threshold is reached and never package
    // has been received, then request a ping-pong test.
    static constexpr int ReceiveTimeout = 10;      // 10ms
    static constexpr int HeartbeatThreshold = 100; // 10ms * 100 = 1s

    int receiveTimeoutCounter = 0;

    std::unordered_map<std::string, // function name
        std::function<void(const std::string&)>> rpcs;

    serializer::FunctionSerializer fsRep; // [Server]
    serializer::FunctionSerializer fsSub; // [Client]

    communicator::ResponderInst  responder;  // [Server]
    communicator::RequesterInst  requester;  // [Client]

    communicator::PublisherInst  publisher;  // [Server]
    communicator::SubscriberInst subscriber; // [Client]

    // params: request, response, process for responder  [Server]
    std::function<void(const std::string&, std::string&)> procRep;
    // params: envelope, content, process for subscriber [Client]
    std::function<void(const std::string&, const std::string&)> procSub;
    // params: received successfully or not, process for subscriber callback.
    std::function<void(bool)> procSubCb;              // [Client]

    Role role = Role::None;
    std::string addrReqRep;
    std::string addrPubSub;
};

}
