#include "random.h"

#include <random>

namespace {
    std::default_random_engine e;
}

double Random::uniform()
{
    
    static std::uniform_real_distribution<double> distribution;
    return distribution(e);
}

double Random::normal(double peak, double spread)
{
    static std::default_random_engine e;
    std::normal_distribution<double> distribution(peak, spread);
    return distribution(e);
}
