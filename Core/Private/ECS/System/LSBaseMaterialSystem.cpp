#include"ECS/System/LSBaseMaterialSystem.h"
#include "LSFileUtil.h"
#include "LSGeometryUtil.h"
#include "LSApplication.h"
#include "Render/LSRenderContext.h"
#include "Render/LSRenderTarget.h"
#include "Graphic/LSVKPipeline.h"
#include "Graphic/LSVKFramebuffer.h"
#include"ECS/Component/LSTransformComponent.h"
#include "ECS/LSEntity.h"
#include "ECS/Component/LSCameraComponent.h"

namespace lse{
    void LSBaseMaterialSystem::OnInit(LSVKRenderPass *renderPass) {

        lse::LSVKDevice *device = GetDevice();

        lse::ShaderLayout shaderLayout = {
            .pushConstants = {
                {
                    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                    .offset = 0,
                    .size = sizeof(PushConstants)
                }
            }
        };
        mPipelineLayout = std::make_shared<LSVKPipelineLayout>(device,
                                                               LS_RES_SHADER_DIR"01_hello_buffer.vert",
                                                               LS_RES_SHADER_DIR"01_hello_buffer.frag",
                                                               shaderLayout);
        std::vector<VkVertexInputBindingDescription> vertexBindings = {
            {
                .binding = 0,
                .stride = sizeof(LSVertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
            }
        };
        std::vector<VkVertexInputAttributeDescription> vertexAttrs = {
            {
                .location = 0,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(LSVertex, position)
            },
            {
                .location = 1,
                .binding = 0,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = offsetof(LSVertex, texcoord)
            },
            {
                .location = 2,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(LSVertex, normal)
            }
        };
        mPipeline = std::make_shared<LSVKPipeline>(device, renderPass, mPipelineLayout.get());
        mPipeline->SetVertexInputState(vertexBindings, vertexAttrs);
        mPipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)->EnableDepthTest();
        mPipeline->SetDynamicState({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR });
        mPipeline->SetMultisampleState(VK_SAMPLE_COUNT_1_BIT, VK_FALSE);
        mPipeline->Create();
    }

    void LSBaseMaterialSystem::OnRender(VkCommandBuffer cmdBuffer,LSRenderTarget *renderTarget) {
        //entt::each
        LSScene *scene = GetScene();

        if(!scene){
            return;
        }

        entt::registry &reg = scene->GetEcsRegistry();
        auto view = reg.view<LSTransformComponent, LSBaseMaterialComponent>();
        if(view.begin() == view.end()){
            return;
        }
        // bind pipeline
        mPipeline->Bind(cmdBuffer);
        // setup global params
        LSVKFramebuffer *frameBuffer = renderTarget->GetFramebuffer();
        VkViewport viewport = {
                .x = 0,
                .y = 0,
                .width = static_cast<float>(frameBuffer->GetWidth()),
                .height = static_cast<float>(frameBuffer->GetHeight()),
                .minDepth = 0.f,
                .maxDepth = 1.f
        };
        vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
        VkRect2D scissor = {
                .offset = { 0, 0 },
                .extent = { frameBuffer->GetWidth(), frameBuffer->GetHeight() }
        };
        vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

        glm::mat4 projMat = GetProjMat(renderTarget);
        glm::mat4 viewMat = GetViewMat(renderTarget);
        LSEntity *camera = renderTarget->GetCamera();
        if(LSEntity::HasComponent<LSCameraComponent>(camera)){
            auto &cameraComp = camera->GetComponent<LSCameraComponent>();
            projMat = cameraComp.GetProjMat();
            viewMat = cameraComp.GetViewMat();
        }

        // setup custom params
        view.each([this, &cmdBuffer, &projMat, &viewMat](const auto &e, const LSTransformComponent &transComp, const LSBaseMaterialComponent &materialComp){
            auto meshMaterial = materialComp.GetMeshMaterials();
            for(const auto &entry : meshMaterial) {
                LSBaseMaterial *material = entry.first;
                if(!material) {
                    LOG_W("TODO: default material or error material");
                    continue;
                }
                PushConstants pushConstants {
                    .matrix = projMat * viewMat * transComp.GetTransform(),
                    .colorType = static_cast<uint32_t>(material->colorType)
                };
                vkCmdPushConstants(cmdBuffer, mPipelineLayout->GetHandle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(pushConstants), &pushConstants);

                for(const auto &meshIndex : entry.second) {
                    LSMesh *mesh = materialComp.GetMesh(meshIndex);
                    if(mesh) {
                        mesh->Draw(cmdBuffer);
                    }
                }

            }
            // mesh list draw

        });

    }
    void LSBaseMaterialSystem::OnDestroy() {
        mPipeline.reset();
        mPipelineLayout.reset();
    }


}