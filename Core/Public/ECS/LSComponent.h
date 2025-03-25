#pragma once

#include"LSEntity.h"

namespace lse {
    class LSComponent{
    public:
        void SetOwner(LSEntity *owner) {mOwner = owner;}
        LSEntity *GetOwner() const {return mOwner;}
    private:
        LSEntity *mOwner = nullptr;
    };


}