#include <LSEntity.h>
#include <LSMeshComponent.h>
#include <LSScene.h>
#include"LSEntryPoint.h"
#include"LSFileUtil.h"
#include"Graphic/LSVKRenderPass.h"
#include"Render/LSRenderTarget.h"
#include"Graphic/LSVKCommandBuffer.h"
#include"Render/LSMesh.h"
#include"Render/LSRenderer.h"
#include"ECS/System/LSBaseMaterialSystem.h"
#include"ECS/Component/LSTransformComponent.h"

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
        mRenderTarget->AddMaterialSystem<lse::LSBaseMaterialSystem>();

        mRenderer = std::make_shared<lse::LSRenderer>();

        mCmdBuffers = device->GetDefaultCmdPool()->AllocateCommandBuffers(swapchain->GetImages().size());

        std::vector<lse::LSVertex> vertices;
        std::vector<uint32_t> indices;
        lse::LSGeometryUtil::CreateCube(-0.3f, 0.3f, -0.3f, 0.3f, -0.3f, 0.3f, vertices, indices);
        mCubeMesh = std::make_shared<lse::LSMesh>(vertices, indices);

    }

    void OnUpdate(float deltaTime) override {

    }

    void OnSceneInit(lse::LSScene *scene) override {
        {
            lse::LSEntity *cube = scene->CreateEntity("Cube 0");
            auto &materialComp = cube->AddComponent<lse::LSBaseMaterialComponent>();
            materialComp.colorType = lse::COLOR_TYPE_TEXCOORD;
            auto &meshComp = cube->AddComponent<lse::LSMeshComponent>();
            meshComp.mMesh = mCubeMesh.get();
            auto &transComp = cube->GetComponent<lse::LSTransformComponent>();
            transComp.scale = { 1.f, 1.f, 1.f };
            transComp.position = { 0.f, 0.f, 0.0f };
            transComp.rotation = { 17.f, 30.f, 0.f };
        }
        {
            lse::LSEntity *cube = scene->CreateEntity("Cube 1");
            auto &materialComp = cube->AddComponent<lse::LSBaseMaterialComponent>();
            materialComp.colorType = lse::COLOR_TYPE_TEXCOORD;
            auto &meshComp = cube->AddComponent<lse::LSMeshComponent>();
            meshComp.mMesh = mCubeMesh.get();
            auto &transComp = cube->GetComponent<lse::LSTransformComponent>();
            transComp.scale = { 0.5f, 0.5f, 0.5f };
            transComp.position = { -1.f, 0.f, 0.0f };
            transComp.rotation = { 17.f, 30.f, 0.f };
        }
        {
            lse::LSEntity *cube = scene->CreateEntity("Cube 2");
            auto &materialComp = cube->AddComponent<lse::LSBaseMaterialComponent>();
            materialComp.colorType = lse::COLOR_TYPE_TEXCOORD;
            auto &meshComp = cube->AddComponent<lse::LSMeshComponent>();
            meshComp.mMesh = mCubeMesh.get();
            auto &transComp = cube->GetComponent<lse::LSTransformComponent>();
            transComp.scale = { 0.5f, 0.5f, 0.5f };
            transComp.position = { 1.f, 0.f, 0.0f };
            transComp.rotation = { 17.f, 30.f, 0.f };
        }
    }

    void OnSceneDestroy(lse::LSScene *scene) override {


    }

    void OnRender() override {
        lse::LSRenderContext *renderContext = LSApplication::GetAppContext()->renderContext;
        lse::LSVKDevice *device = renderContext->GetDevice();
        lse::LSVKSwapchain *swapchain = renderContext->GetSwapchain();

        int32_t imageIndex;
        if(mRenderer->Begin(&imageIndex)) {
            mRenderTarget->SetExtent({swapchain->GetWidth(), swapchain->GetHeight()});
        }

        VkCommandBuffer cmdBuffer = mCmdBuffers[imageIndex];
        lse::LSVKCommandPool::BeginCommandBuffer(cmdBuffer);

        mRenderTarget->Begin(cmdBuffer);

        mRenderTarget->RenderMaterialSystems(cmdBuffer);

        mRenderTarget->End(cmdBuffer);

        lse::LSVKCommandPool::EndCommandBuffer(cmdBuffer);
        if(mRenderer->End(imageIndex,{cmdBuffer})) {
            mRenderTarget->SetExtent({swapchain->GetWidth(), swapchain->GetHeight()});
        }
    }

    void OnDestroy() override {
        lse::LSRenderContext *renderContext = lse::LSApplication::GetAppContext()->renderContext;
        lse::LSVKDevice *device = renderContext->GetDevice();
        vkDeviceWaitIdle(device->GetHandle());
        mCubeMesh.reset();
        mCmdBuffers.clear();
        mRenderTarget.reset();
        mRenderPass.reset();
        mRenderer.reset();
    }

private:
    std::shared_ptr<lse::LSVKRenderPass> mRenderPass;
    std::shared_ptr<lse::LSRenderTarget> mRenderTarget;
    std::shared_ptr<lse::LSRenderer> mRenderer;

    std::vector<VkCommandBuffer> mCmdBuffers;

    std::shared_ptr<lse::LSMesh> mCubeMesh;

 
};

lse::LSApplication *CreateApplicationEntryPoint() {
    return new SandBoxApp();
}