#pragma once

#include "MultiServerSupportBase.hpp"
#include "serializer/FunctionSerializer.hpp"
#include "RpcBasicTemplate.hpp"

namespace ti {

class RpcProcessRequest : public MultiServerSupportBase {
public:
    template <typename R>
    struct CallReturn {
        enum Error {
            Success,
            NetworkTimeout,
            FunctionNotFound
        } error;
        rpc::RpcReturnType<R>::Type value;
    };

    template <typename Retv, typename ...Args>
    CallReturn<Retv> CallFunc(std::string name, Args ...args)
    {
        rpc::RpcFuncArgsWrapper<typename std::decay<Args>::type...>
            wrapper = std::make_tuple(args...);
        std::string request;
        serializer.Serialize(request, name, wrapper);
        return CallRemote<Retv>(request);
    }

protected:
    template <typename Retv>
    inline CallReturn<Retv> CallRemote(const std::string& request)
    {
    }

private:
    serializer::FunctionSerializer serializer;

    communicator::CommunicationCode lastCommCode =
        communicator::CommunicationCode::Success;
    std::string lastCallFuncName;
};

}
