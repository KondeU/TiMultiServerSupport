#pragma once

#include "communicator/Communicator.hpp"

namespace tirpc {

class MultiServerSupportBase {
protected:
    inline static communicator::Communicator& Communicator()
    {
        return communicator;
    }

private:
    static communicator::Communicator communicator;
};

}
