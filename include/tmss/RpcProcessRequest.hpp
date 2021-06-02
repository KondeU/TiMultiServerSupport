#pragma once

#include "MultiServerSupportBase.hpp"
#include "serializer/FunctionSerializer.hpp"
#include "RpcBasicTemplate.hpp"

namespace ti {

class RpcProcessRequest : public MultiServerSupportBase {
public:
    RpcProcessRequest()
    {
        Communicator().ResetInstInvalid(requester);
    }

    bool ConnectNetwork(const std::string& serverIp, int serverPort)
    {
        if (!Communicator().IsInstInvalid(requester)) {
            return false;
        }

        requester = Communicator().CreateRequester(
            serverIp + ":" + std::to_string(serverPort));
        if (Communicator().IsInstInvalid(requester)) {
            return false;
        }

        int timeout = requester->SetTimeout(RpcTimeout);
        if (timeout != RpcTimeout) {
            DisconnectNetwork();
            return false;
        }

        return true;
    }

    bool DisconnectNetwork()
    {
        if (Communicator().IsInstInvalid(requester)) {
            return false;
        }

        Communicator().DestroyInstance(Communicator().MakeInstValue(requester));
        Communicator().ResetInstInvalid(requester);

        return true;
    }

    enum class CallError {
        Success,
        Unknown,
        // If the error type is Network, disconnect network
        // and then connect network again to reset.
        NetworkTimeout,
        // If the error type is Function, check the function
        // name and function binding(RpcProcessResponse::BindFunc).
        FunctionNotFound,
        FunctionNameMismatch
    };

    template <typename T>
    struct CallReturn {
        CallError error;
        typename rpc::RpcReturnType<T>::Type value;
    };

    template <typename Retv, typename ...Args>
    CallReturn<Retv> CallFunc(const std::string& name, const Args& ...args)
    {
        rpc::RpcFuncArgsWrapper<typename std::decay<Args>::type...>
            wrapper = std::make_tuple(args...);

        std::string request;
        serializer.Serialize(request, name, wrapper);

        std::string respond;
        CallError error = CallRemote(request, respond);
        // Filtering error, only Unknown needs to be parsed, see CallRemote.
        if (error == CallError::NetworkTimeout) {
            return { error, {} };
        }

        std::string retFuncName;
        rpc::RpcReturnWrapper<void> retWrapVoid;
        serializer.Deserialize(respond, retFuncName, retWrapVoid);
        // Function name mismatch, may be out-of-order calls occurred.
        if (retFuncName != name) {
            return { CallError::FunctionNameMismatch, {} };
        }
        // Filtering error, only rpc::RpcReturnCode::Success needs to be parsed.
        if (std::get<rpc::RpcReturnCode>(retWrapVoid)
            == rpc::RpcReturnCode::FunctionNotFound) {
            return { CallError::FunctionNotFound, {} };
        }

        CallReturn<Retv> ret{ CallError::Success, {} };
        if (std::is_same<Retv, void>()) {
            return ret;
        }
        rpc::RpcReturnWrapper<Retv> retWrapRetv;
        serializer.Deserialize(respond, retFuncName, retWrapRetv);
        ret.value = std::get<rpc::RpcReturnType<Retv>::Type>(retWrapRetv);
        return ret;
    }

protected:
    inline CallError CallRemote(const std::string& request, std::string& respond)
    {
        switch (requester->Request(request, respond)) {
        case communicator::CommunicationCode::Success:
            return CallError::Unknown;
        case communicator::CommunicationCode::ReceiveTimeout:
            return CallError::NetworkTimeout;
        }
        // It is not possible to run here. Only used to avoid compilation warning.
        return CallError::NetworkTimeout;
    }

private:
    static constexpr int RpcTimeout = 10; // 10ms

    serializer::FunctionSerializer serializer;
    communicator::RequesterInst requester;
};

}
