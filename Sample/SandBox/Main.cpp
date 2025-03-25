#include <LSCameraComponent.h>
#include <LSEntity.h>
#include <LSScene.h>
#include"LSEntryPoint.h"
#include"LSFileUtil.h"
#include"Graphic/LSVKRenderPass.h"
#include"Render/LSRenderTarget.h"
#include "Render/LSMaterial.h"
#include"Graphic/LSVKCommandBuffer.h"
#include"Render/LSMesh.h"
#include"Render/LSRenderer.h"

#include"ECS/System/LSBaseMaterialSystem.h"
#include"ECS/Component/LSTransformComponent.h"
#include"ECS/System/LSUnlitMaterialSystem.h"

#include"LSEventTesting.h"

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
        mRenderTarget->AddMaterialSystem<lse::LSUnlitMaterialSystem>();

        mRenderer = std::make_shared<lse::LSRenderer>();

        mCmdBuffers = device->GetDefaultCmdPool()->AllocateCommandBuffers(swapchain->GetImages().size());

        //Event
        mEventTesting = std::make_shared<LSEventTesting>();
        mObserver = std::make_shared<lse::LSEventObserver>();
        mObserver->OnEvent<lse::LSMouseScrollEvent>([this](const lse::LSMouseScrollEvent &event){
            lse::LSEntity *camera = mRenderTarget->GetCamera();
            if(lse::LSEntity::HasComponent<lse::LSCameraComponent>(camera)){
                auto &cameraComp = camera->GetComponent<lse::LSCameraComponent>();
                float radius = cameraComp.GetRadius() + event.mYOffset * -0.3f;
                if(radius < 0.1f){
                    radius = 0.1f;
                }
                cameraComp.SetRadius(radius);
            }
        });


        std::vector<lse::LSVertex> vertices;
        std::vector<uint32_t> indices;
        lse::LSGeometryUtil::CreateCube(-0.1f, 0.1f, -0.1f, 0.1f, -0.1f, 0.1f, vertices, indices);
        mCubeMesh = std::make_shared<lse::LSMesh>(vertices, indices);

        mDefaultSampler = std::make_shared<lse::LSSampler>(VK_FILTER_NEAREST,VK_SAMPLER_ADDRESS_MODE_REPEAT);
        lse::RGBColor whiteColor = {255,255,255,255};
        lse::RGBColor blackColor = {255,255,255,255};
        lse::RGBColor multiColors[4] = {
            255,255,255,255,
            192,192,192,255,
            192,192,192,255,
            255,255,255,255,
        };
        mWhiteTexture = std::make_shared<lse::LSTexture>(1,1,&whiteColor);
        mBlackTexture = std::make_shared<lse::LSTexture>(1,1,&blackColor);
        mMultiPixelTexture = std::make_shared<lse::LSTexture>(2,2,multiColors);
        mFileTexture = std::make_shared<lse::LSTexture>(LS_RES_TEXTURE_DIR"R-C.png");
    }

    void OnUpdate(float deltaTime) override {
        uint64_t frameIndex = GetFrameIndex();

        lse::LSTexture *textures[] = { mWhiteTexture.get(),mBlackTexture.get(),mFileTexture.get(),mMultiPixelTexture.get()};
        if(frameIndex % 10 == 0 && mUnlitMaterials.size() < mSmallCubes.size()) {
            auto material = lse::LSMaterialFactory::GetInstance() -> CreateMaterial<lse::LSUnlitMaterial>();
            material->SetBaseColor0(glm::linearRand(glm::vec3(0.f,0.f,0.f),glm::vec3(1.f,1.f,1.f)));
            material->SetBaseColor1(glm::linearRand(glm::vec3(0.f,0.f,0.f),glm::vec3(1.f,1.f,1.f)));
            material->SetTextureView(lse::UNLIT_MAT_BASE_COLOR_0,textures[glm::linearRand(0,(int)ARRAY_SIZE(textures)-1)],mDefaultSampler.get());
            material->SetTextureView(lse::UNLIT_MAT_BASE_COLOR_1,textures[glm::linearRand(0,(int)ARRAY_SIZE(textures)-1)],mDefaultSampler.get());
            material->UpdateTextureViewEnable(lse::UNLIT_MAT_BASE_COLOR_0,glm::linearRand(0,1));
            material->UpdateTextureViewEnable(lse::UNLIT_MAT_BASE_COLOR_1,glm::linearRand(0,1));
            material-> SetMixValue(glm::linearRand(0.1f,0.8f));

            uint32_t cubeIndex = mUnlitMaterials.size();
            if(!lse::LSEntity::HasComponent<lse::LSUnlitMaterialComponent>(mSmallCubes[cubeIndex])) {
                mSmallCubes[cubeIndex]->AddComponent<lse::LSUnlitMaterialComponent>();
            }
            auto &materialComp = mSmallCubes[cubeIndex]->GetComponent<lse::LSUnlitMaterialComponent>();
            materialComp.AddMesh(mCubeMesh.get(),material);
            mUnlitMaterials.push_back(material);
            LOG_D("Unlit Material Count: {0}", mUnlitMaterials.size());
        }
        if(frameIndex % 1 == 0 && !mUnlitMaterials.empty()){
            mUnlitMaterials[0]->SetMixValue(glm::linearRand(0.f, 1.f));
            mUnlitMaterials[0]->SetTextureView(lse::UNLIT_MAT_BASE_COLOR_0, textures[glm::linearRand(0, (int)ARRAY_SIZE(textures) - 1)], mDefaultSampler.get());
        }

        //1.视角移动，鼠标左键被按下，表示可以开始移动视角，鼠标左键释放，停止视角移动
        lse::LSEntity *camera = mRenderTarget->GetCamera();
        if(lse::LSEntity::HasComponent<lse::LSCameraComponent>(camera)) {
            if(!mWindow->IsMouseDown()){
                bFirstMouseDrag = true;
                return;
            }

            glm::vec2 mousePos;
            mWindow->GetMousePos(mousePos);
            glm::vec2 mousePosDelta = { mLastMousePos.x - mousePos.x, mLastMousePos.y - mousePos.y };
            mLastMousePos = mousePos;

            if(abs(mousePosDelta.x) > 0.1f || abs(mousePosDelta.y) > 0.1f){
                if(bFirstMouseDrag){
                    bFirstMouseDrag = false;
                } else {
                    auto &transComp = camera->GetComponent<lse::LSTransformComponent>();
                    float yaw = transComp.rotation.x;
                    float pitch = transComp.rotation.y;

                    yaw += mousePosDelta.x * mMouseSensitivity;
                    pitch += mousePosDelta.y * mMouseSensitivity;

                    if(pitch > 89.f){
                        pitch = 89.f;
                    }
                    if(pitch < -89.f){
                        pitch = -89.f;
                    }
                    transComp.rotation.x = yaw;
                    transComp.rotation.y = pitch;
                }
            }
        }
    }

    void OnSceneInit(lse::LSScene *scene) override {
        lse::LSEntity *camera = scene->CreateEntity("Editor Camera");
        camera->AddComponent<lse::LSCameraComponent>();
        mRenderTarget->SetCamera(camera);

        auto baseMaterial0 = lse::LSMaterialFactory::GetInstance()->CreateMaterial<lse::LSBaseMaterial>();
        auto baseMaterial1 = lse::LSMaterialFactory::GetInstance()->CreateMaterial<lse::LSBaseMaterial>();
        baseMaterial1->colorType = lse::COLOR_TYPE_TEXCOORD;

        // mUnlitMaterials[0] = lse::LSMaterialFactory::GetInstance()->CreateMaterial<lse::LSUnlitMaterial>();
        // mUnlitMaterials[0] ->SetTextureView(lse::UNLIT_MAT_BASE_COLOR_0,mFileTexture.get(),mDefaultSampler.get());
        // mUnlitMaterials[0] ->SetTextureView(lse::UNLIT_MAT_BASE_COLOR_1,mMultiPixelTexture.get(),mDefaultSampler.get());
        // mUnlitMaterials[0] ->UpdateTextureViewEnable(lse::UNLIT_MAT_BASE_COLOR_0,true);
        // mUnlitMaterials[0] ->UpdateTextureViewEnable(lse::UNLIT_MAT_BASE_COLOR_1,true);
        // mUnlitMaterials[0] ->UpdateTextureViewUVScale(lse::UNLIT_MAT_BASE_COLOR_1, {2,1});
        // mUnlitMaterials[0] ->SetMixValue(0.2f);

        //uint32_t index = 0;

        float x = -2.f;
        for(int i = 0; i < mSmallCubeSize.x; i++, x+=0.5f){
            float y = -2.f;
            for(int j = 0; j < mSmallCubeSize.y; j++, y+=0.5f){
                float z = -2.f;
                for(int k = 0; k < mSmallCubeSize.z; k++, z+=0.5f){
                    lse::LSEntity *cube = scene->CreateEntity("Cube");
                    // auto &materialComp = cube->AddComponent<lse::LSBaseMaterialComponent>();
                    // materialComp.AddMesh(mCubeMesh.get(),index==0 ? baseMaterial0 : baseMaterial1);

                    //auto &materialComp = cube->AddComponent<lse::LSUnlitMaterialComponent>();
                    //materialComp.AddMesh(mCubeMesh.get(),mUnlitMaterials[0]);
                    auto &transComp = cube->GetComponent<lse::LSTransformComponent>();
                    //transComp.scale = { 1.f, 1.f, 1.f };
                    transComp.position = { x, y, z };
                    //transComp.rotation = { 17.f, 30.f, 0.f };
                    mSmallCubes.push_back(cube);
                    //index = (index +1 ) % 2;
                }
            }
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
        mDefaultSampler.reset();
        mWhiteTexture.reset();
        mBlackTexture.reset();
        mMultiPixelTexture.reset();
        mFileTexture.reset();
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
    std::shared_ptr<lse::LSTexture> mWhiteTexture;
    std::shared_ptr<lse::LSTexture> mBlackTexture;
    std::shared_ptr<lse::LSTexture> mMultiPixelTexture;
    std::shared_ptr<lse::LSTexture> mFileTexture;
    std::shared_ptr<lse::LSSampler> mDefaultSampler;
    glm::ivec3 mSmallCubeSize{10,10,10};  //x, y, z
    std::vector<lse::LSEntity*>mSmallCubes;

    std::vector<lse::LSUnlitMaterial*> mUnlitMaterials;

    std::shared_ptr<LSEventTesting> mEventTesting;
    std::shared_ptr<lse::LSEventObserver> mObserver;


    bool bFirstMouseDrag = true;
    glm::vec2 mLastMousePos;
    float mMouseSensitivity = 0.25f;

};

lse::LSApplication *CreateApplicationEntryPoint() {
    return new SandBoxApp();
}