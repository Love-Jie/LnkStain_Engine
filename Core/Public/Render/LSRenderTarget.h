#pragma once

#include"Graphic/LSVKFramebuffer.h"
#include"Render/LSRenderContext.h"
#include"ECS/LSSystem.h"
#include "ECS/LSEntity.h"
#include "ECS/System/LSMaterialSystem.h"

namespace lse{
    class LSRenderTarget{
    public:
        LSRenderTarget(LSVKRenderPass *renderPass);
        LSRenderTarget(LSVKRenderPass *renderPass,uint32_t bufferCount,VkExtent2D extent);
        ~LSRenderTarget();

        void Begin(VkCommandBuffer cmdBuffer);
        void End(VkCommandBuffer cmdBuffer);

        LSVKRenderPass *GetRenderPass() const { return mRenderPass;}
        LSVKFramebuffer *GetFramebuffer() const { return mFrameBuffers[mCurrentBufferIndex].get();}

        void SetExtent(const VkExtent2D &extent);
        void SetBufferCount(uint32_t bufferCount);

        void SetColorClearValue(VkClearColorValue colorClearValue);
        void SetColorClearValue(uint32_t attachmentIndex, VkClearColorValue colorClearValue);
        void SetDepthStencilClearValue(VkClearDepthStencilValue depthStencilValue);
        void SetDepthStencilClearValue(uint32_t attachmentIndex, VkClearDepthStencilValue depthStencilValue);

        template<typename T, typename... Args>
       void AddMaterialSystem(Args&&... args) {
            std::shared_ptr<LSMaterialSystem> system = std::make_shared<T>(std::forward<Args>(args)...);
            system->OnInit(mRenderPass);
            mMaterialSystemList.push_back(system);
        }

        void RenderMaterialSystems(VkCommandBuffer cmdBuffer) {
            for (auto &item: mMaterialSystemList){
                item->OnRender(cmdBuffer, this);
            }
        }

        void SetCamera(LSEntity *camera){ mCamera =camera;}
        LSEntity *GetCamera() const { return mCamera;}

    private:
        void Init();
        void ReCreate();

        std::vector<std::shared_ptr<LSVKFramebuffer>> mFrameBuffers;


        LSVKRenderPass *mRenderPass;
        std::vector<VkClearValue> mClearValues;
        uint32_t mBufferCount;
        uint32_t mCurrentBufferIndex = 0;
        VkExtent2D mExtent;

        bool bSwapchainTarget = false;
        bool bBeginTarget = false;

        std::vector<std::shared_ptr<LSMaterialSystem>> mMaterialSystemList;
        LSEntity *mCamera;

        bool bShouldUpdate = false;
    };

}