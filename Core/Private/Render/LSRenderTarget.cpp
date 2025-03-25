#include "Render/LSRenderTarget.h"
#include "LSApplication.h"
#include "Graphic/LSVKRenderPass.h"
#include "Graphic/LSVKImage.h"
#include"Graphic/LSVKFramebuffer.h"
#include "ECS/Component/LSCameraComponent.h"

namespace lse {
    LSRenderTarget::LSRenderTarget(LSVKRenderPass *renderPass) {
        LSRenderContext *renderContext = LSApplication::GetAppContext()->renderContext;
        LSVKSwapchain *swapchain = renderContext->GetSwapchain();

        mRenderPass = renderPass;
        mExtent = {swapchain->GetWidth(), swapchain->GetHeight()};
        mBufferCount = swapchain->GetImages().size();
        bSwapchainTarget = true;

        Init();
        ReCreate();
    }

    LSRenderTarget::LSRenderTarget(LSVKRenderPass *renderPass, uint32_t bufferCount, VkExtent2D extent)
                                        :mRenderPass(renderPass),mBufferCount(bufferCount),mExtent(extent),bSwapchainTarget(false){
        Init();
        ReCreate();
    }

    LSRenderTarget::~LSRenderTarget() {
        for (const auto &item: mMaterialSystemList){
            item->OnDestroy();
        }
        mMaterialSystemList.clear();
    }


    void LSRenderTarget::Init() {
        mClearValues.resize(mRenderPass->GetAttachmentSize());
        SetColorClearValue({ 0.f, 0.f, 0.f, 1.f });
        SetDepthStencilClearValue({ 1.f, 0 });
    }

    void LSRenderTarget::ReCreate() {
        if(mExtent.width == 0 || mExtent.height == 0) {
            return;
        }

        mFrameBuffers.clear();
        mFrameBuffers.resize(mBufferCount);

        LSRenderContext *renderContext = LSApplication::GetAppContext()->renderContext;
        LSVKDevice *device = renderContext->GetDevice();
        LSVKSwapchain *swapchain = renderContext->GetSwapchain();

        std::vector<Attachment> attachments = mRenderPass->GetAttachments();
        if(attachments.empty()){
            return;
        }

        std::vector<VkImage> swapchainImages = swapchain->GetImages();

        for(int i = 0; i < mBufferCount; i++){
            std::vector<std::shared_ptr<LSVKImage>> images;
            for(int j = 0; j < attachments.size(); j++){
                Attachment attachment = attachments[j];
                if(bSwapchainTarget && attachment.finalLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR && attachment.samples == VK_SAMPLE_COUNT_1_BIT){
                    images.push_back(std::make_shared<LSVKImage>(device, swapchainImages[i], VkExtent3D{ mExtent.width, mExtent.height, 1 }, attachment.format, attachment.usage));
                } else {
                    images.push_back(std::make_shared<LSVKImage>(device, VkExtent3D{ mExtent.width, mExtent.height, 1 }, attachment.format, attachment.usage));
                }
            }
            mFrameBuffers[i] = std::make_shared<LSVKFramebuffer>(device, mRenderPass, images, mExtent.width, mExtent.height);
            images.clear();
        }
    }

    void LSRenderTarget::Begin(VkCommandBuffer cmdBuffer) {
        assert(!bBeginTarget && "You should not called Begin() again.");

        if(bShouldUpdate) {
            ReCreate();
            bShouldUpdate = false;
        }

        if(LSEntity::HasComponent<LSCameraComponent>(mCamera)){
            mCamera->GetComponent<LSCameraComponent>().SetAspect(mExtent.width * 1.f / mExtent.height);
        }

        if(bSwapchainTarget) {
            LSRenderContext *renderContext = LSApplication::GetAppContext()->renderContext;
            LSVKSwapchain *swapchain = renderContext->GetSwapchain();
            mCurrentBufferIndex = swapchain->GetCurrentImageIndex();
        }else {
            mCurrentBufferIndex = (mCurrentBufferIndex + 1) % mBufferCount;
        }

        mRenderPass->BeginRenderPass(cmdBuffer,GetFramebuffer(),mClearValues);
        bBeginTarget = true;

    }

    void LSRenderTarget::End(VkCommandBuffer cmdBuffer) {
        if(bBeginTarget){
            mRenderPass->EndRenderPass(cmdBuffer);
            bBeginTarget = false;
        }
    }

    void LSRenderTarget::SetExtent(const VkExtent2D &extent) {
        mExtent = extent;
        bShouldUpdate = true;
    }

    void LSRenderTarget::SetBufferCount(uint32_t bufferCount) {
        mBufferCount = bufferCount;
        bShouldUpdate = true;
    }

    void LSRenderTarget::SetColorClearValue(VkClearColorValue colorClearValue) {
        std::vector<Attachment> renderPassAttachments = mRenderPass->GetAttachments();
        for(int i = 0; i < renderPassAttachments.size(); i++){
            if(!IsDepthStencilFormat(renderPassAttachments[i].format) && renderPassAttachments[i].loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR){
                mClearValues[i].color = colorClearValue;
            }
        }
    }

    void LSRenderTarget::SetDepthStencilClearValue(VkClearDepthStencilValue depthStencilValue) {
        std::vector<Attachment> renderPassAttachments = mRenderPass->GetAttachments();
        for(int i = 0; i < renderPassAttachments.size(); i++){
            if(IsDepthStencilFormat(renderPassAttachments[i].format) && renderPassAttachments[i].loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR){
                mClearValues[i].depthStencil = depthStencilValue;
            }
        }
    }

    void LSRenderTarget::SetColorClearValue(uint32_t attachmentIndex, VkClearColorValue colorClearValue) {
        std::vector<Attachment> renderPassAttachments = mRenderPass->GetAttachments();
        if(attachmentIndex <= renderPassAttachments.size() - 1){
            if(!IsDepthStencilFormat(renderPassAttachments[attachmentIndex].format) && renderPassAttachments[attachmentIndex].loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR){
                mClearValues[attachmentIndex].color = colorClearValue;
            }
        }
    }

    void LSRenderTarget::SetDepthStencilClearValue(uint32_t attachmentIndex, VkClearDepthStencilValue depthStencilValue) {
        std::vector<Attachment> renderPassAttachments = mRenderPass->GetAttachments();
        if(attachmentIndex <= renderPassAttachments.size() - 1){
            if(IsDepthStencilFormat(renderPassAttachments[attachmentIndex].format) && renderPassAttachments[attachmentIndex].loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR){
                mClearValues[attachmentIndex].depthStencil = depthStencilValue;
            }
        }
    }
}



