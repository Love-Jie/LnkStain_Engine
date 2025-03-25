#pragma once

#include"LSMaterialComponent.h"

namespace lse{
    enum BaseMaterialColor{
        COLOR_TYPE_NORMAL = 0,
        COLOR_TYPE_TEXCOORD = 1
    };

    struct LSBaseMaterial : public LSMaterial{
        BaseMaterialColor colorType;
    };

    struct LSBaseMaterialComponent : public LSMaterialComponent<LSBaseMaterial>{

    };

}