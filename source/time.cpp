#include "time.h"

long long Time::timestamp()
{
    return Time::milliseconds(std::chrono::system_clock::now().time_since_epoch());
}