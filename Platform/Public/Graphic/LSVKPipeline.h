#pragma once
#include"LSVKCommon.h"
#include"Graphic/LSVKRenderPass.h"

namespace lse {
	class LSVKDevice;

	//定义shader布局结构体
	struct ShaderLayout {
		std::vector<VkDescriptorSetLayout>descriptorSetLayouts;
		std::vector<VkPushConstantRange>pushConstants;
	};

	//VkPipelineVertexInputStateCreateInfo aa = {};

	struct PipelineVertexInputState{
		std::vector<VkVertexInputBindingDescription>vertexBindingDescriptions;
		std::vector<VkVertexInputAttributeDescription>vertexAttributes;
	};

	struct PipelineInputAssemblyState{
		VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST ;    //图元的类型
		VkBool32 primitiveRestartEnable = VK_FALSE;
	};

	struct PipelineRasterizationState{
		VkBool32 depthClampEnable = VK_FALSE;
		VkBool32 rasterizerDiscardEnable = VK_FALSE;
		VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
		VkCullModeFlags cullMode = VK_CULL_MODE_NONE;
        VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        VkBool32 depthBiasEnable = VK_FALSE;
    	float depthBiasConstantFactor = 0.0f;
        float depthBiasSlopeFactor = 0.0f;
        float depthBiasClamp = 0.0f;
		float lineWidth = 1.0f;
    };

	struct PipelineMultisampleState{
		VkSampleCountFlagBits rasterizetionSample = VK_SAMPLE_COUNT_1_BIT;   //采样次数
		VkBool32 sampleShadingEnable = VK_FALSE;
     	float minSampleShading = 0.2f;
	};

	struct PipelineDepthStencilState{
		VkBool32 depthTestEnable = VK_FALSE;   //默认不开启深度测试
 		VkBool32 depthWriteEnable = VK_FALSE;   //默认不开启深度数据的写入
   		VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS;
        VkBool32 depthBoundstextEnable = VK_FALSE;
        VkBool32 stencilTestEnable = VK_FALSE;
    };

	struct PipelineDynamicState{
		std::vector<VkDynamicState> dynamicStates;
    };

	struct PipelineConfig{
		PipelineVertexInputState vertexInputState;
		PipelineInputAssemblyState inputAssemblyState;
     	PipelineRasterizationState rasterizationState;
    	PipelineMultisampleState multisampleState;
        PipelineDepthStencilState depthStencilState;
  		VkPipelineColorBlendAttachmentState colorBlendAttachmentState{
  			.blendEnable = VK_FALSE,
  			.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
  			.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
  			.colorBlendOp =  VK_BLEND_OP_ADD,
  			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
  			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
  			.alphaBlendOp = VK_BLEND_OP_ADD,
  			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
  		};
		PipelineDynamicState dynamicState;
    };

	class LSVKPipelineLayout {
	public:
		LSVKPipelineLayout(LSVKDevice* device, const std::string& vertexShaderFile, const std::string& fragShaderFile, const ShaderLayout& shaderLayout = {});
		~LSVKPipelineLayout();

		VkPipelineLayout GetHandle() const { return mHandle; };
		VkShaderModule GetVertexShaderModule() const { return mVertexShaderModule; };
		VkShaderModule GetFragShaderModule() const { return mFragShaderModule; };

	private:
		VkResult CompileShaderModule(const std::string& filePath, VkShaderModule* outShaderModule);

		LSVKDevice* mDevice;
		VkPipelineLayout mHandle = VK_NULL_HANDLE;
		VkShaderModule mVertexShaderModule = VK_NULL_HANDLE;
		VkShaderModule mFragShaderModule = VK_NULL_HANDLE;
	};


	class LSVKPipeline {
	public:
		LSVKPipeline(LSVKDevice* device,LSVKRenderPass *renderPass,LSVKPipelineLayout *pipelineLayout);
		~LSVKPipeline();

		void Create();

		void Bind(VkCommandBuffer cmdBuffer);

		LSVKPipeline *SetVertexInputState(const std::vector<VkVertexInputBindingDescription>& vertexBindings,const std::vector<VkVertexInputAttributeDescription> &vertexAttris);
		LSVKPipeline *SetInputAssemblyState(VkPrimitiveTopology topology,VkBool32 primitiveRestarEnable=VK_FALSE);
		LSVKPipeline *SetRasterzationState(const PipelineRasterizationState &rasterzationState);
		LSVKPipeline *SetMultisampleState(VkSampleCountFlagBits samples,VkBool32 sampleShadingEnable,float minSampleShading = 0.f );
		LSVKPipeline *SetDepthStencilState(const PipelineDepthStencilState &depthStencilState);
		LSVKPipeline *SetColorBlendAttachmentState(VkBool32 blendEnable,
													VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ONE,VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,
													VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,VkBlendFactor dstAlphaBlendFactor=VK_BLEND_FACTOR_ZERO,VkBlendOp alphaBlendOp =  VK_BLEND_OP_ADD);
		LSVKPipeline *SetDynamicState(const std::vector<VkDynamicState>& dynamicStates);
		LSVKPipeline *EnableAlphaBlend();
		LSVKPipeline *EnableDepthTest();

		VkPipeline GetHandle() const { return mHandle; };
	private:

		LSVKDevice* mDevice;
		LSVKRenderPass *mRenderPass;
		LSVKPipelineLayout *mPipelineLayout;
		VkPipeline mHandle = VK_NULL_HANDLE;

		PipelineConfig mPipelineConfig;
	};


}