#pragma once

#include"Graphic/LSVKCommon.h"

namespace lse{
    class LSVKRenderPass;
    class LSRenderTarget;
    class LSSystem {
    public:
        // 更新系统状态（由引擎调用，独立于渲染线程）
        virtual void OnUpdate(float deltaTime) {};
    };


}