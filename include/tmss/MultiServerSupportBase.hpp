#pragma once

#include "communicator/Communicator.hpp"

namespace ti {

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
