#include<iostream>

#include"LSLog.h"
#include"LSWindow.h"
#include"Graphic/LSGraphicContext.h"
#include"Graphic/LSVKGraphicContext.h"
#include"Graphic/LSVKDevice.h"
#include"Graphic/LSVKSwapchain.h"
#include"Graphic/LSVKFramebuffer.h"
#include"Graphic/LSVKRenderPass.h"
#include"Graphic/LSVKImage.h"
#include"Graphic/LSVKPipeline.h"
#include"LSFileUtil.h"
#include"Graphic/LSVKCommandBuffer.h"
#include"Graphic/LSVKQueue.h"
#include"LSGeometryUtil.h"
#include"Graphic/LSVKBuffer.h"

struct PushConstants {
    glm::mat4 matrix{1.f};
};

int main() {
    std::cout<<"Hello LnkStain Engine!"<<std::endl;

    lse::LSLog::Init();

    std::unique_ptr<lse::LSWindow> window = lse::LSWindow::Create(800, 600, "FirstCube");
    std::unique_ptr<lse::LSGraphicContext>graphicContext = lse::LSGraphicContext::Create(window.get());
    auto vkContext = dynamic_cast<lse::LSVKGraphicContext*>(graphicContext.get());
    std::shared_ptr<lse::LSVKDevice> device = std::make_shared<lse::LSVKDevice>(dynamic_cast<lse::LSVKGraphicContext*>(graphicContext.get()),1,1);
    std::shared_ptr<lse::LSVKSwapchain> swapchain = std::make_shared<lse::LSVKSwapchain>(vkContext,device.get());
    swapchain->ReCreate();

    //TODO query depth format
    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;

    //renderpass
    std::vector<VkAttachmentDescription> attachments = {};
    //color attachment
    attachments.emplace_back();
    attachments[0].flags = 0;
    attachments[0].format = device->GetSettings().surfaceFormat;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    //depth attachment
    attachments.emplace_back();
    attachments[1].flags = 0;
    attachments[1].format = depthFormat;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    std::vector<lse::RenderSubPass> subpasses = {};
    subpasses.emplace_back();
    subpasses[0].colorAttachment = {0,VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    subpasses[0].depthStencilAttachment = {1,VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

    std::shared_ptr<lse::LSVKRenderPass> renderPass = std::make_shared<lse::LSVKRenderPass>(device.get(),attachments,subpasses);

    //获取交换链中的图片，并为每一张图片都分配一个framebuffer
    std::vector<VkImage>swapchainImages = swapchain->GetImages();
    uint32_t swapchainImageSize = swapchainImages.size();
    std::vector<std::shared_ptr<lse::LSVKFramebuffer>>framebuffers;
    VkExtent3D imageExtent = {swapchain->GetWidth(), swapchain->GetHeight(), 1};
    for(uint32_t i = 0; i < swapchainImageSize; i++) {
        std::vector<std::shared_ptr<lse::LSVKImage>> images = {
            std::make_shared<lse::LSVKImage>(device.get(),swapchainImages[i],imageExtent,device->GetSettings().surfaceFormat,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT),
            std::make_shared<lse::LSVKImage>(device.get(),imageExtent,depthFormat,VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        };
        framebuffers.push_back(std::make_shared<lse::LSVKFramebuffer>(device.get(), renderPass.get(), images, swapchain->GetWidth(), swapchain->GetHeight()));
    }

    //用于pipelineLayout中定义的shaderlayout的pushConstants常量
    lse::ShaderLayout shaderLayout = {};
    shaderLayout.pushConstants.emplace_back();
    shaderLayout.pushConstants[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    shaderLayout.pushConstants[0].offset = 0;
    shaderLayout.pushConstants[0].size = sizeof(PushConstants);

    std::shared_ptr<lse::LSVKPipelineLayout> pipelineLayout = std::make_shared<lse::LSVKPipelineLayout>(device.get(),
                                                        LS_RES_SHADER_DIR"01_hello_buffer.vert",LS_RES_SHADER_DIR"01_hello_buffer.frag",shaderLayout);

    std::shared_ptr<lse::LSVKPipeline> pipeline = std::make_shared<lse::LSVKPipeline>(device.get(),renderPass.get(), pipelineLayout.get());
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

    pipeline->SetVertexInputState(vertexBindings,vertexAttrs);
    pipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)->EnableDepthTest();
    pipeline->SetDynamicState({VK_DYNAMIC_STATE_VIEWPORT,VK_DYNAMIC_STATE_SCISSOR});
    pipeline->Create();

    std::shared_ptr<lse::LSVKCommandPool> cmdPool = std::make_shared<lse::LSVKCommandPool>(device.get(),vkContext->GetGraphicQueueFamilyInfo().queueFamilyIndex);
    std::vector<VkCommandBuffer> cmdBuffers = cmdPool->AllocateCommandBuffers(swapchainImages.size());

    //geometry
    std::vector<lse::LSVertex> vertices;
    std::vector<uint32_t> indices;
    lse::LSGeometryUtil::CreateCube(-0.3f,0.3f,-0.3f,0.3f,-0.3f,0.3f,vertices,indices);
    std::shared_ptr<lse::LSVKBuffer> vertexBuffer = std::make_shared<lse::LSVKBuffer>(device.get(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                                                    sizeof(vertices[0]) * vertices.size(), vertices.data());
    std::shared_ptr<lse::LSVKBuffer> indexBuffer = std::make_shared<lse::LSVKBuffer>(device.get(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                                                    sizeof(indices[0]) * indices.size(), indices.data());
    PushConstants pc{};
    std::cout<<"vertex size:"<<vertices.size()<<std::endl;
    std::cout<<"indices size:" << indices.size()<<std::endl;

    lse::LSVKQueue *graphicQueue = device->GetFirstGraphicQueue();

    const std::vector<VkClearValue> clearValues = {
        {0.1f,0.2f,0.3f,1.f},
        {1,0}
    };

    //1.acquire swapchain image Semaphore
    //2.submited Semaphore
    //3.frame fence

    const uint32_t numBuffer = 2;
    std::vector<VkSemaphore> imageAvailabeSemaphores(numBuffer);
    std::vector<VkSemaphore> submitedSemaphores(numBuffer);
    std::vector<VkFence> frameFences(numBuffer);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(int i = 0; i < numBuffer; i++) {
        CALL_VK(vkCreateSemaphore(device->GetHandle(),&semaphoreInfo,nullptr,&imageAvailabeSemaphores[i]));
        CALL_VK(vkCreateSemaphore(device->GetHandle(),&semaphoreInfo,nullptr,&submitedSemaphores[i]));
        CALL_VK(vkCreateFence(device->GetHandle(),&fenceInfo,nullptr,&frameFences[i]));
    }

    std::chrono::time_point lastTimePoint = std::chrono::steady_clock::now();

    uint32_t currentBuffer = 0;

    while(!window->ShouldClose()) {
        window->PollEvents();

        CALL_VK(vkWaitForFences(device->GetHandle(),1,&frameFences[currentBuffer],VK_FALSE,UINT64_MAX));
        CALL_VK(vkResetFences(device->GetHandle(),1,&frameFences[currentBuffer]));

        //1.acquire swapchain image
        int32_t imageIndex = swapchain->AcquireImage(imageAvailabeSemaphores[currentBuffer]);

        float time = std::chrono::duration<float>(std::chrono::steady_clock::now() - lastTimePoint).count();
        pc.matrix = glm::rotate(glm::mat4(1.0f), glm::radians(-17.f), glm::vec3(1.0f, 0.0f, 0.0f));
        pc.matrix = glm::rotate(pc.matrix,glm::radians(time * 100.f),glm::vec3(0,1,0));

        pc.matrix = glm::ortho(-1.f,1.f,-1.f,1.f,-1.f,1.f) *pc.matrix;

        //2.begin commandbuffer
        lse::LSVKCommandPool::BeginCommandBuffer(cmdBuffers[imageIndex]);

        //3.begin renderpass bind framebuffer
        renderPass->BeginRenderPass(cmdBuffers[imageIndex],framebuffers[imageIndex].get(),clearValues);

        //4.bind resource -> pipeline,geometry,descriptorset
        pipeline->Bind(cmdBuffers[imageIndex]);

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(framebuffers[imageIndex]->GetWidth());
        viewport.height = static_cast<float>(framebuffers[imageIndex]->GetHeight());
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;
        vkCmdSetViewport(cmdBuffers[imageIndex], 0, 1, &viewport);

        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = {framebuffers[imageIndex]->GetWidth(), framebuffers[imageIndex]->GetHeight()};
        vkCmdSetScissor(cmdBuffers[imageIndex], 0, 1, &scissor);

        vkCmdPushConstants(cmdBuffers[imageIndex],pipelineLayout->GetHandle(),VK_SHADER_STAGE_VERTEX_BIT,0,sizeof(pc),&pc);

        VkBuffer vertexBuffers[] = { vertexBuffer->GetHandle() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(cmdBuffers[imageIndex],0,1,vertexBuffers,offsets);

        vkCmdBindIndexBuffer(cmdBuffers[imageIndex],indexBuffer->GetHandle(),0,VK_INDEX_TYPE_UINT32);

        //5.draw
        //vkCmdDraw(cmdBuffers[imageIndex],3,1,0,0);
        vkCmdDrawIndexed(cmdBuffers[imageIndex],indices.size(),1,0,0,0);

        //6.end renderpass
        renderPass->EndRenderPass(cmdBuffers[imageIndex]);

        //7.end commandbuffer
        lse::LSVKCommandPool::EndCommandBuffer(cmdBuffers[imageIndex]);

        //8.submit commandbuffer to queue
        graphicQueue->submit({cmdBuffers[imageIndex]},{imageAvailabeSemaphores[currentBuffer]},{submitedSemaphores[currentBuffer]},frameFences[currentBuffer]);
        graphicQueue->WaitIdle();

        //9.present
        swapchain->Present(imageIndex,{submitedSemaphores[currentBuffer]});

        window->SwapBuffer();

        currentBuffer = (currentBuffer + 1) % numBuffer;

    }

    for(int i = 0; i < numBuffer; i++) {
        vkDeviceWaitIdle(device->GetHandle());
        VK_D(Semaphore,device->GetHandle(),imageAvailabeSemaphores[i]);
        VK_D(Semaphore,device->GetHandle(),submitedSemaphores[i]);
        VK_D(Fence,device->GetHandle(),frameFences[i]);
    }

    return EXIT_SUCCESS;
}