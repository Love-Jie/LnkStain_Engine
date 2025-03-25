#pragma once

#include <glm/fwd.hpp>

#include "Graphic/LSVKCommon.h"
#include "LSGraphicContext.h"
#include "ECS/LSSystem.h"

namespace lse{
    class LSVKRenderPass;
    class LSRenderTarget;
    class LSApplication;
    class LSScene;
    class LSVKDevice;

    class LSMaterialSystem : public LSSystem{
    public:
        virtual void OnInit(LSVKRenderPass *renderPass) = 0;
        virtual void OnRender(VkCommandBuffer cmdBuffer, LSRenderTarget *renderTarget) = 0;
        virtual void OnDestroy() = 0;
    protected:


        LSApplication *GetApp() const;
        LSScene *GetScene() const;
        LSVKDevice *GetDevice() const;
        const glm::mat4 GetProjMat(LSRenderTarget *renderTarget) const;
        const glm::mat4 GetViewMat(LSRenderTarget *renderTarget) const;
    };

}