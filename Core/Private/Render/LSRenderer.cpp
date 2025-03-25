#include"Render/LSRenderer.h"
#include"LSApplication.h"

namespace lse{
    LSRenderer::LSRenderer() {
        lse::LSRenderContext *renderContext = lse::LSApplication::GetAppContext()->renderContext;
        lse::LSVKDevice *device = renderContext->GetDevice();

        mImageAvailableSemaphores.resize(RENDER_NUM_BUFFER);
        mSubmitedSemaphores.resize(RENDER_NUM_BUFFER);
        mFrameFences.resize(RENDER_NUM_BUFFER);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreInfo.pNext = nullptr;
        semaphoreInfo.flags = 0;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = nullptr;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for(int i = 0; i < RENDER_NUM_BUFFER; i++){
            CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]));
            CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &mSubmitedSemaphores[i]));
            CALL_VK(vkCreateFence(device->GetHandle(), &fenceInfo, nullptr, &mFrameFences[i]));
        }
    }

    LSRenderer::~LSRenderer() {
        lse::LSRenderContext *renderContext = lse::LSApplication::GetAppContext()->renderContext;
        lse::LSVKDevice *device = renderContext->GetDevice();
        for(const auto& item : mImageAvailableSemaphores) {
            VK_D(Semaphore,device->GetHandle(),item);
        }
        for(const auto& item : mSubmitedSemaphores) {
            VK_D(Semaphore,device->GetHandle(),item);
        }
        for(const auto& item : mFrameFences) {
            VK_D(Fence,device->GetHandle(),item);
        }
    }

    bool LSRenderer::Begin(int32_t *outImageIndex) {
        lse::LSRenderContext *renderContext = LSApplication::GetAppContext()->renderContext;
        lse::LSVKDevice *device = renderContext->GetDevice();
        lse::LSVKSwapchain *swapchain = renderContext->GetSwapchain();

        bool bShouleUpdateTarget = false;

        CALL_VK(vkWaitForFences(device->GetHandle(),1,&mFrameFences[mCurrentBuffer],VK_FALSE,UINT64_MAX));
        CALL_VK(vkResetFences(device->GetHandle(),1,&mFrameFences[mCurrentBuffer]));

        VkResult ret = swapchain->AcquireImage(outImageIndex,mImageAvailableSemaphores[mCurrentBuffer]);
        if(ret == VK_ERROR_OUT_OF_DATE_KHR){
            CALL_VK(vkDeviceWaitIdle(device->GetHandle()));
            VkExtent2D originExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
            bool bSuc = swapchain->ReCreate();

            VkExtent2D newExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
            if(bSuc && (originExtent.width != newExtent.width || originExtent.height != newExtent.height)){
                bool bSuc = true;
            }
            ret = swapchain->AcquireImage(outImageIndex, mImageAvailableSemaphores[mCurrentBuffer]);
            if(ret != VK_SUCCESS && ret != VK_SUBOPTIMAL_KHR){
                LOG_E("Recreate swapchain error: {0}", vk_result_string(ret));
            }
        }
        return bShouleUpdateTarget;
    }

    bool LSRenderer::End(int32_t imageIndex,const std::vector<VkCommandBuffer> &cmdBuffers) {
        lse::LSRenderContext *renderContext = LSApplication::GetAppContext()->renderContext;
        lse::LSVKDevice *device = renderContext->GetDevice();
        lse::LSVKSwapchain *swapchain = renderContext->GetSwapchain();

        bool bShouleUpdateTarget = false;

        device->GetFirstGraphicQueue()->submit(cmdBuffers, { mImageAvailableSemaphores[mCurrentBuffer] }, { mSubmitedSemaphores[mCurrentBuffer] }, mFrameFences[mCurrentBuffer]);

        VkResult ret = swapchain->Present(imageIndex,{ mSubmitedSemaphores[mCurrentBuffer]} );

        if(ret == VK_SUBOPTIMAL_KHR){
            CALL_VK(vkDeviceWaitIdle(device->GetHandle()));
            VkExtent2D originExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
            bool bSuc = swapchain->ReCreate();

            VkExtent2D newExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
            if(bSuc && (originExtent.width != newExtent.width || originExtent.height != newExtent.height)){
               bShouleUpdateTarget = true;
            }
        }
        CALL_VK(vkDeviceWaitIdle(device->GetHandle()));
        mCurrentBuffer = (mCurrentBuffer+1) % RENDER_NUM_BUFFER;
        return bShouleUpdateTarget;
    }
}