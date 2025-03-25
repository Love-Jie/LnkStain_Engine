#include "ECS/System/LSUnlitMaterialSystem.h"

#include "LSFileUtil.h"
#include "LSApplication.h"
#include "Graphic/LSVKPipeline.h"
#include "Graphic/LSVKDescriptorSet.h"
#include "Graphic/LSVKImageView.h"
#include "Graphic/LSVKFrameBuffer.h"

#include "Render/LSRenderTarget.h"

#include "ECS/Component/LSTransformComponent.h"


namespace lse{
    void LSUnlitMaterialSystem::OnInit(LSVKRenderPass *renderPass) {
        LSVKDevice *device = GetDevice();
        //frame Ubo
        {
            const std::vector<VkDescriptorSetLayoutBinding>bindings = {
                {
                    .binding = 0,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                }
            };
            mFrameUboDescriptorSetLayout = std::make_shared<LSVKDescriptorSetLayout>(device,bindings);
        }

        //Material Params
        {
            const std::vector<VkDescriptorSetLayoutBinding> bindings = {
                {
                    .binding = 0,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
                }
            };
            mMaterialParamDescriptorSetLayout = std::make_shared<LSVKDescriptorSetLayout>(device, bindings);
        }

        // Material Resource
        {
            const std::vector<VkDescriptorSetLayoutBinding> bindings = {
                {
                    .binding = 0,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
                },
                {
                    .binding = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
                }
            };
            mMaterialResourceDescriptorSetLayout = std::make_shared<LSVKDescriptorSetLayout>(device, bindings);
        }

        VkPushConstantRange modelPC = {};
        modelPC.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        modelPC.offset = 0;
        modelPC.size = sizeof(ModelPC);

        ShaderLayout shaderLayout = {};
        shaderLayout.descriptorSetLayouts = {mFrameUboDescriptorSetLayout->GetHandle(),mMaterialParamDescriptorSetLayout->GetHandle(),mMaterialResourceDescriptorSetLayout->GetHandle()};
        shaderLayout.pushConstants = { modelPC };

        mPipelineLayout = std::make_shared<LSVKPipelineLayout>(device,
                                                                LS_RES_SHADER_DIR"03_unlit_material.vert",
                                                                LS_RES_SHADER_DIR"03_unlit_material.frag",
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

        mPipeline = std::make_shared<LSVKPipeline>(device,renderPass,mPipelineLayout.get());
        mPipeline->SetVertexInputState(vertexBindings, vertexAttrs);
        mPipeline->EnableDepthTest();
        mPipeline->SetDynamicState({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR });
        mPipeline->SetMultisampleState(VK_SAMPLE_COUNT_1_BIT, VK_FALSE);
        mPipeline->Create();

        std::vector<VkDescriptorPoolSize> poolSizes = {
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1
            }
        };
        mDescriptorPool = std::make_shared<LSVKDescriptorPool>(device, 1, poolSizes);
        mFrameUboDescriptorSet = mDescriptorPool->AllocateDescriptorSet(mFrameUboDescriptorSetLayout.get(), 1)[0];
        mFrameUboBuffer = std::make_shared<lse::LSVKBuffer>(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(FrameUbo), nullptr, true);

        ReCreateMaterialDescriptorPool(NUM_MATERIAL_BATCH);

    }

    void LSUnlitMaterialSystem::OnRender(VkCommandBuffer cmdBuffer, LSRenderTarget *renderTarget) {
        LSScene *scene = GetScene();
        if(!scene) {
            return;
        }
        entt::registry &reg = scene->GetEcsRegistry();
        auto view = reg.view<LSTransformComponent,LSUnlitMaterialComponent>();
        if(view.begin() == view.end()) {
            return;
        }

        mPipeline->Bind(cmdBuffer);
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

        UpdateFrameUboDescriptorSet(renderTarget);

        bool bShouldForceUpdateMaterial = false;
        uint32_t materialCount = LSMaterialFactory::GetInstance()->GetMaterialSize<LSUnlitMaterial>();
        if(materialCount > mLastDescriptorSetCount){
            ReCreateMaterialDescriptorPool(materialCount);
            bShouldForceUpdateMaterial = true;
        }

        std::vector<bool>updateFlags(materialCount);
        view.each([this,&updateFlags,&bShouldForceUpdateMaterial, &cmdBuffer](LSTransformComponent &transComp,LSUnlitMaterialComponent &materialComp) {
           for(const auto &entry : materialComp.GetMeshMaterials()) {
               LSUnlitMaterial *material = entry.first;
               if(!material || material->GetIndex() < 0) {
                   LOG_W("TODO: default material or error material ?");
                   continue;
               }

               uint32_t materialIndex = material->GetIndex();
               VkDescriptorSet paramsDescSet = mMaterialDescriptorSets[materialIndex];
               VkDescriptorSet resourceDescSet = mMaterialResourceDescriptorSets[materialIndex];

               if(!updateFlags[materialIndex]) {
                    if(material->ShouldFlushParams() || bShouldForceUpdateMaterial) {
                        UpdateMaterialParamsDescriptorSet(paramsDescSet, material);
                    }
                   if(material->ShouldFlushResource() || bShouldForceUpdateMaterial){
                        //LOG_T("Update material resource : {0}", materialIndex);
                        UpdateMaterialResourceDescriptorSet(resourceDescSet, material);
                        material->FinishFlushResource();
                    }
                   updateFlags[materialIndex] = true;
               }

               VkDescriptorSet descriptorSets[] = { mFrameUboDescriptorSet, paramsDescSet, resourceDescSet };
               vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->GetHandle(),
                                        0, ARRAY_SIZE(descriptorSets), descriptorSets, 0, nullptr);

               ModelPC pc = { transComp.GetTransform() };
               vkCmdPushConstants(cmdBuffer, mPipelineLayout->GetHandle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(pc), &pc);

               for (const auto &meshIndex: entry.second) {
                    materialComp.GetMesh(meshIndex)->Draw(cmdBuffer);
               }
           }
        });
    }

    void LSUnlitMaterialSystem::OnDestroy() {
    }

    void LSUnlitMaterialSystem::ReCreateMaterialDescriptorPool(uint32_t materialCount) {
        LSVKDevice *device = GetDevice();

        uint32_t newDescriptorSetCount = mLastDescriptorSetCount;
        if(mLastDescriptorSetCount == 0) {
            newDescriptorSetCount = NUM_MATERIAL_BATCH;
        }

        while(newDescriptorSetCount < materialCount) {
            newDescriptorSetCount *= 2;
        }

        if(newDescriptorSetCount > NUM_MATERIAL_BATCH_MAX) {
            LOG_E("Descriptor Set max count is : {0}, but request : {1}", NUM_MATERIAL_BATCH_MAX, newDescriptorSetCount);
            return;
        }

        LOG_W("{0}: {1} -> {2} S.", __FUNCTION__, mLastDescriptorSetCount, newDescriptorSetCount);

        // Destroy old
        mMaterialDescriptorSets.clear();
        mMaterialResourceDescriptorSets.clear();
        if(mMaterialDescriptorPool){
            mMaterialDescriptorPool.reset();
        }

        std::vector<VkDescriptorPoolSize> poolSizes = {
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = newDescriptorSetCount
            },
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = newDescriptorSetCount * 2               // because has color_tex0 and color_tex1
            }
        };
        mMaterialDescriptorPool = std::make_shared<lse::LSVKDescriptorPool>(device,newDescriptorSetCount * 2, poolSizes);

        mMaterialDescriptorSets = mMaterialDescriptorPool->AllocateDescriptorSet(mMaterialParamDescriptorSetLayout.get(), newDescriptorSetCount);
        mMaterialResourceDescriptorSets = mMaterialDescriptorPool->AllocateDescriptorSet(mMaterialResourceDescriptorSetLayout.get(), newDescriptorSetCount);
        assert(mMaterialDescriptorSets.size() == newDescriptorSetCount && "Failed to AllocateDescriptorSet");
        assert(mMaterialResourceDescriptorSets.size() == newDescriptorSetCount && "Failed to AllocateDescriptorSet");

        uint32_t diffCount = newDescriptorSetCount - mLastDescriptorSetCount;
        for(int i = 0; i < diffCount; i++){
            mMaterialBuffers.push_back(std::make_shared<LSVKBuffer>(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UnlitMaterialUbo), nullptr, true));
        }
        LOG_W("{0}: {1} -> {2} E.", __FUNCTION__, mLastDescriptorSetCount, newDescriptorSetCount);
        mLastDescriptorSetCount = newDescriptorSetCount;
    }

    void LSUnlitMaterialSystem::UpdateFrameUboDescriptorSet(LSRenderTarget *renderTarget) {
        LSApplication *app = GetApp();
        LSVKDevice *device = GetDevice();

        LSVKFramebuffer *frameBuffer = renderTarget->GetFramebuffer();
        glm::ivec2 resolution = { frameBuffer->GetWidth(), frameBuffer->GetHeight() };

        FrameUbo frameUbo = {
            .projMat = GetProjMat(renderTarget),
            .viewMat = GetViewMat(renderTarget),
            .resolution = resolution,
            .frameId = static_cast<uint32_t>(app->GetFrameIndex()),
            .time = app->GetStartTimeSecond()
        };

        mFrameUboBuffer->WriteData(&frameUbo);
        VkDescriptorBufferInfo bufferInfo = DescriptorSetWriter::BuildBufferInfo(mFrameUboBuffer->GetHandle(), 0, sizeof(frameUbo));
        VkWriteDescriptorSet bufferWrite = DescriptorSetWriter::WriteBuffer(mFrameUboDescriptorSet, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo);
        DescriptorSetWriter::UpdateDescriptorSets(device->GetHandle(), { bufferWrite });
    }

    void LSUnlitMaterialSystem::UpdateMaterialParamsDescriptorSet(VkDescriptorSet descSet, LSUnlitMaterial *material) {
        LSVKDevice *device = GetDevice();

        LSVKBuffer *materialBuffer = mMaterialBuffers[material->GetIndex()].get();

        UnlitMaterialUbo params = material->GetParams();

        const TextureView *texture0 = material->GetTextureView(UNLIT_MAT_BASE_COLOR_0);
        if(texture0){
            LSMaterial::UpdateTextureParams(texture0, &params.textureParam0);
        }

        const TextureView *texture1 = material->GetTextureView(UNLIT_MAT_BASE_COLOR_1);
        if(texture1){
            LSMaterial::UpdateTextureParams(texture1, &params.textureParam1);
        }

        materialBuffer->WriteData(&params);
        VkDescriptorBufferInfo bufferInfo = DescriptorSetWriter::BuildBufferInfo(materialBuffer->GetHandle(), 0, sizeof(params));
        VkWriteDescriptorSet bufferWrite = DescriptorSetWriter::WriteBuffer(descSet, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo);
        DescriptorSetWriter::UpdateDescriptorSets(device->GetHandle(), { bufferWrite });
    }

    void LSUnlitMaterialSystem::UpdateMaterialResourceDescriptorSet(VkDescriptorSet descSet, LSUnlitMaterial *material) {
        LSVKDevice *device = GetDevice();

        const TextureView *texture0 = material->GetTextureView(UNLIT_MAT_BASE_COLOR_0);
        const TextureView *texture1 = material->GetTextureView(UNLIT_MAT_BASE_COLOR_1);

        VkDescriptorImageInfo textureInfo0 = DescriptorSetWriter::BuildImageInfo(texture0->sampler->GetHandle(), texture0->texture->GetImageView()->GetHandle());
        VkDescriptorImageInfo textureInfo1 = DescriptorSetWriter::BuildImageInfo(texture1->sampler->GetHandle(), texture1->texture->GetImageView()->GetHandle());

        VkWriteDescriptorSet textureWrite0 = DescriptorSetWriter::WriteImage(descSet, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &textureInfo0);
        VkWriteDescriptorSet textureWrite1 = DescriptorSetWriter::WriteImage(descSet, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &textureInfo1);

        DescriptorSetWriter::UpdateDescriptorSets(device->GetHandle(), { textureWrite0, textureWrite1 });
    }
}
