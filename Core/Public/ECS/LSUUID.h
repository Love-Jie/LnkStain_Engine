#pragma once

#include"LSEngine.h"

namespace lse{
    class LSUUID{
    public:
        LSUUID();
        LSUUID(uint32_t uuid);
        LSUUID(const LSUUID&) = default;
        operator uint32_t() const {return mUUID;};
        uint64_t mUUID;
    };
}

namespace std{
    template<>
    struct hash<lse::LSUUID>{
        std::size_t operator()(const lse::LSUUID& uuid) const{
             if(!uuid){
                 return 0;
             }
             return (uint32_t)uuid;
        }
    };
}