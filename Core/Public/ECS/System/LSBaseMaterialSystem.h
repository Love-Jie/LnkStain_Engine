#pragma once

#include "ECS/System/LSMaterialSystem.h"
#include "ECS/Component/LSTransformComponent.h"
#include "ECS/Component/Material/LSBaseMaterialComponent.h"
#include "LSGraphicContext.h"

namespace lse{
    class LSVKPipelineLayout;
    class LSVKPipeline;

    struct PushConstants{
        glm::mat4 matrix{ 1.f };
        uint32_t colorType;
    };

    class LSBaseMaterialSystem : public LSMaterialSystem{
    public:
        void OnInit(LSVKRenderPass *renderPass) override;
        void OnRender(VkCommandBuffer cmdBuffer, LSRenderTarget *renderTarget) override;
        void OnDestroy() override;
    private:
        std::shared_ptr<LSVKPipelineLayout> mPipelineLayout;
        std::shared_ptr<LSVKPipeline> mPipeline;
    };

}