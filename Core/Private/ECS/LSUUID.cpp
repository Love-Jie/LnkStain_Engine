#include"ECS/LSUUID.h"
#include<random>

namespace lse{
    static std::random_device s_RandomDevice;
    static std::mt19937_64 sEngine(s_RandomDevice());
    static std::uniform_int_distribution<uint64_t> s_UniformDistribution(1,UINT32_MAX);

    LSUUID::LSUUID():mUUID(s_UniformDistribution(sEngine)){

    }

    LSUUID::LSUUID(uint32_t uuid):mUUID(uuid){

    }
}