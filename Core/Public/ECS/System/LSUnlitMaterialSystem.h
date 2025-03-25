#pragma once

#include "ECS/System/LSMaterialSystem.h"
#include "ECS/Component/Material/LSUnlitMaterialComponent.h"

namespace lse{
#define NUM_MATERIAL_BATCH      16
#define NUM_MATERIAL_BATCH_MAX  2048

    class LSVKPipelineLayout;
    class LSVKPipeline;
    class LSVKDescriptorSetLayout;
    class LSVKDescriptorPool;

    class LSUnlitMaterialSystem : public LSMaterialSystem{
    public:
        public:
        void OnInit(LSVKRenderPass *renderPass) override;
        void OnRender(VkCommandBuffer cmdBuffer, LSRenderTarget *renderTarget) override;
        void OnDestroy() override;
    private:
        void ReCreateMaterialDescriptorPool(uint32_t materialCount);
        void UpdateFrameUboDescriptorSet(LSRenderTarget *renderTarget);
        void UpdateMaterialParamsDescriptorSet(VkDescriptorSet descSet, LSUnlitMaterial *material);
        void UpdateMaterialResourceDescriptorSet(VkDescriptorSet descSet, LSUnlitMaterial *material);

        std::shared_ptr<LSVKDescriptorSetLayout> mFrameUboDescriptorSetLayout;
        std::shared_ptr<LSVKDescriptorSetLayout> mMaterialParamDescriptorSetLayout;
        std::shared_ptr<LSVKDescriptorSetLayout> mMaterialResourceDescriptorSetLayout;

        std::shared_ptr<LSVKPipelineLayout> mPipelineLayout;
        std::shared_ptr<LSVKPipeline> mPipeline;

        std::shared_ptr<LSVKDescriptorPool> mDescriptorPool;
        std::shared_ptr<LSVKDescriptorPool> mMaterialDescriptorPool;

        VkDescriptorSet mFrameUboDescriptorSet;
        std::shared_ptr<LSVKBuffer>mFrameUboBuffer;

        uint32_t mLastDescriptorSetCount = 0;
        std::vector<VkDescriptorSet> mMaterialDescriptorSets;
        std::vector<VkDescriptorSet> mMaterialResourceDescriptorSets;
        std::vector<std::shared_ptr<LSVKBuffer>> mMaterialBuffers;
    };


}

