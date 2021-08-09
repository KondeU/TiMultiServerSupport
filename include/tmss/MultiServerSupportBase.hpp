#pragma once

#include "Singleton.hpp"
#include "communicator/Communicator.hpp"

namespace tirpc {

class MultiServerSupportBase {
protected:
    inline communicator::Communicator& Communicator()
    {
        return communicator();
    }

private:
    common::Singleton<communicator::Communicator> communicator;
};

}
