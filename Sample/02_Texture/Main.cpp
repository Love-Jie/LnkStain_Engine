#include <LSTexture.h>

#include"LSEntryPoint.h"
#include"LSFileUtil.h"
#include"LSGeometryUtil.h"
#include"Graphic/LSVKRenderPass.h"
#include"Render/LSRenderTarget.h"
#include"Graphic/LSVKPipeline.h"
#include"Graphic/LSVKCommandBuffer.h"
#include"Render/LSMesh.h"
#include"Graphic/LSVKQueue.h"
#include"Graphic/LSVKDescriptorSet.h"

struct GlobalUbo {
    glm::mat4 projMat{1.f};
    glm::mat4 viewMat{1.f};
};

struct InstanceUbo {
    glm::mat4 modelMat{1.f};
};

class SandBoxApp : public lse::LSApplication {
protected:
    void OnConfiguration(lse::AppSettings *appSettings) override {
        appSettings->width = 1360;
        appSettings->height = 768;
        appSettings->title = "SandBox";
    }

    void OnInit() override {
        lse::LSRenderContext *renderContext = LSApplication::GetAppContext()->renderContext;
        lse::LSVKDevice *device = renderContext->GetDevice();
        lse::LSVKSwapchain *swapchain = renderContext->GetSwapchain();

        std::vector<lse::Attachment> attachments = {
            {
                .format = swapchain->GetSurfaceInfo().surfaceFormat.format,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
            },
            {
                .format = device->GetSettings().depthFormat,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
            }
        };
        std::vector<lse::RenderSubPass> subpasses = {
            {
                .colorAttachments = { 0 },
                .depthStencilAttachments = { 1 },
                .sampleCount = VK_SAMPLE_COUNT_1_BIT
            }
        };

        mRenderPass = std::make_shared<lse::LSVKRenderPass>(device,attachments,subpasses);
        mRenderTarget = std::make_shared<lse::LSRenderTarget>(mRenderPass.get());
        mRenderTarget->SetColorClearValue({0.1f, 0.2f, 0.3f, 1.f});
        mRenderTarget->SetDepthStencilClearValue({ 1, 0 });

        //descriptor set
        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = {};
        descriptorSetLayoutBindings.emplace_back();
        descriptorSetLayoutBindings[0].binding = 0;
        descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorSetLayoutBindings[0].descriptorCount = 1;
        descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        descriptorSetLayoutBindings.emplace_back();
        descriptorSetLayoutBindings[1].binding = 1;
        descriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorSetLayoutBindings[1].descriptorCount = 1;
        descriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        descriptorSetLayoutBindings.emplace_back();
        descriptorSetLayoutBindings[2].binding = 2;
        descriptorSetLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorSetLayoutBindings[2].descriptorCount = 1;
        descriptorSetLayoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        descriptorSetLayoutBindings.emplace_back();
        descriptorSetLayoutBindings[3].binding = 3;
        descriptorSetLayoutBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorSetLayoutBindings[3].descriptorCount = 1;
        descriptorSetLayoutBindings[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        mDescriptorSetLayout = std::make_shared<lse::LSVKDescriptorSetLayout>(device, descriptorSetLayoutBindings);

        lse::ShaderLayout shaderLayout = {};
        shaderLayout.descriptorSetLayouts = {mDescriptorSetLayout ->GetHandle()};


        mPipelineLayout = std::make_shared<lse::LSVKPipelineLayout>(device,
                                        LS_RES_SHADER_DIR"02_descriptor_set.vert",LS_RES_SHADER_DIR"02_descriptor_set.frag",shaderLayout);


        std::vector<VkVertexInputBindingDescription> vertexBindings = {};
        vertexBindings.emplace_back();
        vertexBindings[0].binding = 0;
        vertexBindings[0].stride = sizeof(lse::LSVertex);
        vertexBindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        std::vector<VkVertexInputAttributeDescription> vertexAttrs = {};
        vertexAttrs.emplace_back();
        vertexAttrs[0].location = 0;
        vertexAttrs[0].binding = 0;
        vertexAttrs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexAttrs[0].offset = offsetof(lse::LSVertex, position);
        vertexAttrs.emplace_back();
        vertexAttrs[1].location = 1;
        vertexAttrs[1].binding = 0;
        vertexAttrs[1].format = VK_FORMAT_R32G32_SFLOAT;
        vertexAttrs[1].offset = offsetof(lse::LSVertex, texcoord);
        vertexAttrs.emplace_back();
        vertexAttrs[2].location = 2;
        vertexAttrs[2].binding = 0;
        vertexAttrs[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexAttrs[2].offset = offsetof(lse::LSVertex, normal);

        std::shared_ptr<lse::LSVKPipeline> pipeline = std::make_shared<lse::LSVKPipeline>(device,mRenderPass.get(), mPipelineLayout.get());
        mPipeline = std::make_shared<lse::LSVKPipeline>(device, mRenderPass.get(), mPipelineLayout.get());
        mPipeline->SetVertexInputState(vertexBindings, vertexAttrs);
        mPipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)->EnableDepthTest();
        mPipeline->SetDynamicState({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR });
        mPipeline->SetMultisampleState(VK_SAMPLE_COUNT_1_BIT, VK_FALSE);
        mPipeline->Create();

        std::vector<VkDescriptorPoolSize> poolSizes = {
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 2
            },
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 2
            }
        };

        mDescriptorPool = std::make_shared<lse::LSVKDescriptorPool>(device, 1, poolSizes);
        mDescriptorSets = mDescriptorPool->AllocateDescriptorSet(mDescriptorSetLayout.get(), 1);

        mGlobalBuffer = std::make_shared<lse::LSVKBuffer>(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(GlobalUbo), nullptr, true);
        mInstanceBuffer = std::make_shared<lse::LSVKBuffer>(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(InstanceUbo), nullptr, true);
        mTexture0 = std::make_shared<lse::LSTexture>(LS_RES_TEXTURE_DIR"awesomeface.png");
        mTexture1 = std::make_shared<lse::LSTexture>(LS_RES_TEXTURE_DIR"R-C.png");


        mImageAvailableSemaphores.resize(mNumBuffer);
        mSubmitedSemaphores.resize(mNumBuffer);
        mFrameFences.resize(mNumBuffer);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreInfo.pNext = nullptr;
        semaphoreInfo.flags = 0;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = nullptr;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for(int i = 0; i < mNumBuffer; i++){
            CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]));
            CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreInfo, nullptr, &mSubmitedSemaphores[i]));
            CALL_VK(vkCreateFence(device->GetHandle(), &fenceInfo, nullptr, &mFrameFences[i]));
        }

        mCmdBuffers = device->GetDefaultCmdPool()->AllocateCommandBuffers(swapchain->GetImages().size());
        std::vector<lse::LSVertex> vertices;
        std::vector<uint32_t> indices;
        lse::LSGeometryUtil::CreateCube(-0.3f, 0.3f, -0.3f, 0.3f, -0.3f, 0.3f, vertices, indices);
        mCubeMesh = std::make_shared<lse::LSMesh>(vertices, indices);


    }

    void OnUpdate(float deltaTime) override {
        lse::LSRenderContext *renderContext = LSApplication::GetAppContext()->renderContext;
        lse::LSVKSwapchain *swapchain = renderContext->GetSwapchain();

        float time = std::chrono::duration<float>(std::chrono::steady_clock::now() - mStartTimePoint).count();
        mInstanceUbo.modelMat = glm::rotate(glm::mat4(1.f), glm::radians(-17.f), glm::vec3(1, 0, 0));
        mInstanceUbo.modelMat = glm::rotate(mInstanceUbo.modelMat, glm::radians(time * 100.f), glm::vec3(0, 1, 0));
        mGlobalUbo.projMat[1][1] *= -1.f;
        mGlobalUbo.projMat = glm::perspective(glm::radians(65.f), swapchain->GetWidth() * 1.f / swapchain->GetHeight(), 0.01f, 100.f);
        mGlobalUbo.viewMat = glm::lookAt(glm::vec3{ 0, 0, 1.5f }, glm::vec3{ 0, 0, -1 }, glm::vec3{ 0, 1, 0 });

    }

    void OnRender() override {
        lse::LSRenderContext *renderContext = LSApplication::GetAppContext()->renderContext;
        lse::LSVKDevice *device = renderContext->GetDevice();
        lse::LSVKSwapchain *swapchain = renderContext->GetSwapchain();

        CALL_VK(vkWaitForFences(device->GetHandle(),1,&mFrameFences[mCurrentBuffer],VK_FALSE,UINT64_MAX));
        CALL_VK(vkResetFences(device->GetHandle(),1,&mFrameFences[mCurrentBuffer]));

        int32_t imageIndex;
        VkResult ret = swapchain->AcquireImage(&imageIndex,mImageAvailableSemaphores[mCurrentBuffer]);
        if(ret == VK_ERROR_OUT_OF_DATE_KHR){
            CALL_VK(vkDeviceWaitIdle(device->GetHandle()));
            VkExtent2D originExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
            bool bSuc = swapchain->ReCreate();

            VkExtent2D newExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
            if(bSuc && (originExtent.width != newExtent.width || originExtent.height != newExtent.height)){
                mRenderTarget->SetExtent(newExtent);
            }
            ret = swapchain->AcquireImage(&imageIndex, mImageAvailableSemaphores[mCurrentBuffer]);
            if(ret != VK_SUCCESS && ret != VK_SUBOPTIMAL_KHR){
                LOG_E("Recreate swapchain error: {0}", vk_result_string(ret));
            }
        }

        VkCommandBuffer cmdBuffer = mCmdBuffers[imageIndex];
        lse::LSVKCommandPool::BeginCommandBuffer(cmdBuffer);

        mRenderTarget->Begin(cmdBuffer);
        lse::LSVKFramebuffer *frameBuffer = mRenderTarget->GetFramebuffer();

        mPipeline->Bind(cmdBuffer);

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(frameBuffer->GetWidth());
        viewport.height = static_cast<float>(frameBuffer->GetHeight());
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;
        vkCmdSetViewport(mCmdBuffers[imageIndex], 0, 1, &viewport);

        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = {frameBuffer->GetWidth(), frameBuffer->GetHeight()};
        vkCmdSetScissor(mCmdBuffers[imageIndex], 0, 1, &scissor);

        mGlobalBuffer->WriteData(&mGlobalUbo);
        mInstanceBuffer->WriteData(&mInstanceUbo);
        UpdateDescriptorSets();

        vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->GetHandle(),
                                0, 1, mDescriptorSets.data(), 0, nullptr);

        mCubeMesh->Draw(cmdBuffer);

        mRenderTarget->End(cmdBuffer);

        lse::LSVKCommandPool::EndCommandBuffer(cmdBuffer);
        device->GetFirstGraphicQueue()->submit({ cmdBuffer }, { mImageAvailableSemaphores[mCurrentBuffer] }, { mSubmitedSemaphores[mCurrentBuffer] }, mFrameFences[mCurrentBuffer]);
        ret = swapchain->Present(imageIndex, { mSubmitedSemaphores[mCurrentBuffer] });
        if(ret == VK_SUBOPTIMAL_KHR){
            CALL_VK(vkDeviceWaitIdle(device->GetHandle()));
            VkExtent2D originExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
            bool bSuc = swapchain->ReCreate();

            VkExtent2D newExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
            if(bSuc && (originExtent.width != newExtent.width || originExtent.height != newExtent.height)){
                mRenderTarget->SetExtent(newExtent);
            }
        }

        mCurrentBuffer = (mCurrentBuffer+1) % mNumBuffer;

    }

    void OnDestroy() override {
        lse::LSRenderContext *renderContext = lse::LSApplication::GetAppContext()->renderContext;
        lse::LSVKDevice *device = renderContext->GetDevice();
        vkDeviceWaitIdle(device->GetHandle());
        mGlobalBuffer.reset();
        mInstanceBuffer.reset();
        mTexture0.reset();
        mTexture1.reset();
        mCubeMesh.reset();
        mCmdBuffers.clear();
        mDescriptorPool.reset();
        mDescriptorSetLayout.reset();
        mPipeline.reset();
        mPipelineLayout.reset();
        mRenderTarget.reset();
        mRenderPass.reset();
        for(int i = 0; i < mNumBuffer; i++){
            VK_D(Semaphore, device->GetHandle(), mImageAvailableSemaphores[i]);
            VK_D(Semaphore, device->GetHandle(), mSubmitedSemaphores[i]);
            VK_D(Fence, device->GetHandle(), mFrameFences[i]);
        }
    }

    void UpdateDescriptorSets() {
        lse::LSRenderContext *renderContext = lse::LSApplication::GetAppContext()->renderContext;
        lse::LSVKDevice *device = renderContext->GetDevice();

        VkDescriptorBufferInfo globalBufferInfo = {
            .buffer = mGlobalBuffer->GetHandle(),
            .offset = 0,
            .range = sizeof(mGlobalUbo)
        };

        VkDescriptorBufferInfo instanceBufferInfo = {
            .buffer = mInstanceBuffer->GetHandle(),
            .offset = 0,
            .range = sizeof(mInstanceUbo)
        };

        VkDescriptorImageInfo textureImageInfo0 = {
            .sampler = mTexture0->GetSampler(),
            .imageView = mTexture0->GetImageView()->GetHandle(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        VkDescriptorImageInfo textureImageInfo1 = {
            .sampler = mTexture1->GetSampler(),
            .imageView = mTexture1->GetImageView()->GetHandle(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        VkDescriptorSet descriptorSet = mDescriptorSets[0];
        std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSet,
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &globalBufferInfo
            },
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSet,
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &instanceBufferInfo
            },
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSet,
                .dstBinding = 2,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &textureImageInfo0
            },
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = descriptorSet,
                .dstBinding = 3,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &textureImageInfo1
            },
        };
        vkUpdateDescriptorSets(device->GetHandle(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
    }

private:
    std::shared_ptr<lse::LSVKRenderPass> mRenderPass;
    std::shared_ptr<lse::LSRenderTarget> mRenderTarget;
    std::shared_ptr<lse::LSVKDescriptorSetLayout> mDescriptorSetLayout;
    std::shared_ptr<lse::LSVKDescriptorPool> mDescriptorPool;
    std::vector<VkDescriptorSet> mDescriptorSets;
    std::shared_ptr<lse::LSVKPipelineLayout> mPipelineLayout;
    std::shared_ptr<lse::LSVKPipeline> mPipeline;
    std::vector<VkCommandBuffer> mCmdBuffers;

    std::shared_ptr<lse::LSMesh> mCubeMesh;
    GlobalUbo mGlobalUbo;
    InstanceUbo mInstanceUbo;
    std::shared_ptr<lse::LSVKBuffer> mGlobalBuffer;
    std::shared_ptr<lse::LSVKBuffer> mInstanceBuffer;
    std::shared_ptr<lse::LSTexture> mTexture0;
    std::shared_ptr<lse::LSTexture> mTexture1;

    const uint32_t mNumBuffer = 2;
    uint32_t mCurrentBuffer = 0;
    std::vector<VkSemaphore> mImageAvailableSemaphores;
    std::vector<VkSemaphore> mSubmitedSemaphores;
    std::vector<VkFence> mFrameFences;
 
};

lse::LSApplication *CreateApplicationEntryPoint() {
    return new SandBoxApp();
}