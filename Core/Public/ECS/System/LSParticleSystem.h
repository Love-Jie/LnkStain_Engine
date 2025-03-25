#pragma once

#include "ECS/LSSystem.h"
#include "ECS/Component/LSTransformComponent.h"
#include"LSGraphicContext.h"

namespace lse{
    class LSVKPipelineLayout;
    class LSVKPipeline;

    // 粒子系统 Push Constants（传递给着色器的常量数据）
    struct ParticlePushConstants {
        glm::mat4 viewMatrix;   // 视图矩阵
        glm::mat4 projMatrix;   // 投影矩阵
        //uint32_t frameIndex;    // 帧索引（用于动画）
    };

    // 粒子数据结构（SOA 方式组织）
    struct ParticleData {
        glm::vec3* positions;     // 所有粒子的位置
        glm::vec3* velocities;     // 所有粒子的速度
        float* lives;            // 粒子生命周期（0~1）
        float* sizes;            // 粒子尺寸
        glm::vec4* colors;       // 粒子颜色
        uint32_t* indices;       // 索引缓冲区（用于三角剖分）
        uint32_t count;           // 当前活跃粒子数
        uint32_t capacity;        // 最大粒子容量
    };

    class LSParticleSystem : public LSSystem{
    public:
        LSParticleSystem();
        ~LSParticleSystem();

        // 初始化粒子系统（创建 Vulkan 资源）
        void OnInit(LSVKRenderPass *renderPass) override;

        // 渲染粒子（使用实例化绘制）
        void OnRender(VkCommandBuffer cmdBuffer, LSRenderTarget* renderTarget) override;

        // 更新粒子状态（CPU 或 GPU 计算）
        void Update(float deltaTime);

        // 创建新粒子
        uint32_t CreateParticle(const glm::vec3& startPos, const glm::vec3& initialVel,
                               float life, const glm::vec4& color, float size);

        // 获取粒子数据访问器（供着色器使用）
        ParticleData& GetParticleData() { return mParticleData; }

    private:
        // Vulkan 资源
        std::shared_ptr<LSVKPipelineLayout> mPipelineLayout;
        std::shared_ptr<LSVKPipeline> mPipeline;
        VkBuffer mVertexBuffer;       // 顶点缓冲区（SOA 数据）
        VkBuffer mIndexBuffer;        // 索引缓冲区
        VkDeviceMemory mVertexBufferMem;
        VkDeviceMemory mIndexBufferMem;

        // 粒子数据（SOA 结构）
        ParticleData mParticleData;

        // 工具函数
        void BuildPipeline(LSVKRenderPass* renderPass);
        void UpdateVertexBuffer();
        void RecycleParticles();

    };

}