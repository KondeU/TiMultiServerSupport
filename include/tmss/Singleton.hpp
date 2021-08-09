#pragma once

namespace tirpc {
namespace common {

template <typename T>
class Singleton {
public:
    static T& GetReference()
    {
        static T instance{};
        return instance;
    }

    T& operator()() const
    {
        return reference;
    }

private:
    static T& reference;
};

template <typename T>
T& Singleton<T>::reference = Singleton<T>::GetReference();

}
}
