#ifndef TIME_H_
#define TIME_H_

#include <chrono>

namespace Time {
    long long timestamp();

    template<typename T>
    long long milliseconds(T duration)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }
}

#endif