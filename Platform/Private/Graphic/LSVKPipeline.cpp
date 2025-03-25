#include"Graphic/LSVKPipeline.h"
#include"LSFileUtil.h"
#include"Graphic/LSVKDevice.h"
#include"Graphic/LSVKRenderPass.h"

namespace lse {
	LSVKPipelineLayout::LSVKPipelineLayout(LSVKDevice* device, const std::string& vertexShaderFile, const std::string& fragShaderFile, const ShaderLayout& shaderLayout ):mDevice(device) {
		//compile Shader
		CALL_VK(CompileShaderModule(vertexShaderFile + ".spv",&mVertexShaderModule));
		CALL_VK(CompileShaderModule(fragShaderFile + ".spv",&mFragShaderModule));

		//pipeline layout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pNext = nullptr;
		pipelineLayoutInfo.flags = 0;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(shaderLayout.descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = shaderLayout.descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(shaderLayout.pushConstants.size());
		pipelineLayoutInfo.pPushConstantRanges = shaderLayout.pushConstants.data();

		CALL_VK(vkCreatePipelineLayout(mDevice->GetHandle(),&pipelineLayoutInfo,nullptr,&mHandle));

		LOG_I("pipeline layout: {0}",(void*)mHandle);
	}

	LSVKPipelineLayout::~LSVKPipelineLayout() {
		VK_D(ShaderModule,mDevice->GetHandle(),mVertexShaderModule);
		VK_D(ShaderModule,mDevice->GetHandle(),mFragShaderModule);
		VK_D(PipelineLayout,mDevice->GetHandle(),mHandle);
	}

	//用于编译着色器模块
	VkResult LSVKPipelineLayout::CompileShaderModule(const std::string& filePath, VkShaderModule* outShaderModule) {
		std::vector<char>content = ReadCharArrayFromFile(filePath);
		VkShaderModuleCreateInfo shaderModuleInfo = {};
		shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleInfo.pNext = nullptr;
		shaderModuleInfo.flags = 0;
		shaderModuleInfo.codeSize = static_cast<uint32_t>(content.size());
		shaderModuleInfo.pCode = reinterpret_cast<const uint32_t *>(content.data());

		return vkCreateShaderModule(mDevice->GetHandle(),&shaderModuleInfo,nullptr,outShaderModule);
	}

	//////pipeline
	LSVKPipeline::LSVKPipeline(LSVKDevice *device, LSVKRenderPass *renderPass, LSVKPipelineLayout *pipelineLayout) : mDevice(device),mRenderPass(renderPass),mPipelineLayout(pipelineLayout) {

	}

	LSVKPipeline::~LSVKPipeline() {
		if (mHandle != VK_NULL_HANDLE) {
			vkDestroyPipeline(mDevice->GetHandle(), mHandle, nullptr);
			mHandle = VK_NULL_HANDLE; // 重置句柄
		}

	}

	void LSVKPipeline::Create() {
		VkPipelineShaderStageCreateInfo shaderStageInfo[] = {

			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			   .pNext = nullptr,
			   .flags = 0,
			   .stage = VK_SHADER_STAGE_VERTEX_BIT,
			   .module = mPipelineLayout->GetVertexShaderModule(),
			   .pName = "main",
			   .pSpecializationInfo = nullptr
			},
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			   .pNext = nullptr,
			   .flags = 0,
			   .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			   .module = mPipelineLayout->GetFragShaderModule(),
			   .pName = "main",
			   .pSpecializationInfo = nullptr
			}
		};

		VkPipelineVertexInputStateCreateInfo vertexInputStageInfo = {};
		vertexInputStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStageInfo.pNext = nullptr;
		vertexInputStageInfo.flags = 0;
		vertexInputStageInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(mPipelineConfig.vertexInputState.vertexBindingDescriptions.size());
		vertexInputStageInfo.pVertexBindingDescriptions = mPipelineConfig.vertexInputState.vertexBindingDescriptions.data();
		vertexInputStageInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(mPipelineConfig.vertexInputState.vertexAttributes.size());
		vertexInputStageInfo.pVertexAttributeDescriptions = mPipelineConfig.vertexInputState.vertexAttributes.data();

		if (vertexInputStageInfo.vertexBindingDescriptionCount == 0) {
			vertexInputStageInfo.pVertexBindingDescriptions = nullptr;
		}
		if (vertexInputStageInfo.vertexAttributeDescriptionCount == 0) {
			vertexInputStageInfo.pVertexAttributeDescriptions = nullptr;
		}

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStageInfo = {};
		inputAssemblyStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyStageInfo.pNext = nullptr;
		inputAssemblyStageInfo.flags = 0;
		inputAssemblyStageInfo.topology = mPipelineConfig.inputAssemblyState.topology;
		inputAssemblyStageInfo.primitiveRestartEnable = mPipelineConfig.inputAssemblyState.primitiveRestartEnable;

		//默认的窗口宽高，后续需要动态修改
		VkViewport defaultViewport = {};
		defaultViewport.x = 0;
		defaultViewport.y = 0;
		defaultViewport.width = 100;
		defaultViewport.height = 100;
		defaultViewport.minDepth = 0;
		defaultViewport.maxDepth = 1;

		VkRect2D defaultScissor = {};
		defaultScissor.offset = {0, 0};
		defaultScissor.extent = {100,100};


		VkPipelineViewportStateCreateInfo viewportStateInfo = {};
		viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateInfo.pNext = nullptr;
		viewportStateInfo.flags = 0;
		viewportStateInfo.viewportCount = 1;
		viewportStateInfo.pViewports = &defaultViewport;
		viewportStateInfo.scissorCount = 1;
		viewportStateInfo.pScissors = &defaultScissor;

		VkPipelineRasterizationStateCreateInfo rasterizerStateInfo = {};
		rasterizerStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerStateInfo.flags = 0;
		rasterizerStateInfo.depthClampEnable = mPipelineConfig.rasterizationState.depthClampEnable;
		rasterizerStateInfo.rasterizerDiscardEnable = mPipelineConfig.rasterizationState.rasterizerDiscardEnable;
		rasterizerStateInfo.polygonMode = mPipelineConfig.rasterizationState.polygonMode;
		rasterizerStateInfo.cullMode = mPipelineConfig.rasterizationState.cullMode;
		rasterizerStateInfo.frontFace = mPipelineConfig.rasterizationState.frontFace;
		rasterizerStateInfo.depthBiasEnable = mPipelineConfig.rasterizationState.depthBiasEnable;
		rasterizerStateInfo.depthBiasConstantFactor = mPipelineConfig.rasterizationState.depthBiasConstantFactor;
		rasterizerStateInfo.depthBiasClamp = mPipelineConfig.rasterizationState.depthBiasClamp;
		rasterizerStateInfo.depthBiasSlopeFactor = mPipelineConfig.rasterizationState.depthBiasSlopeFactor;
		rasterizerStateInfo.lineWidth = mPipelineConfig.rasterizationState.lineWidth;

		VkPipelineMultisampleStateCreateInfo multisampleStateInfo = {};
		multisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateInfo.pNext = nullptr;
		multisampleStateInfo.flags = 0;
		multisampleStateInfo.rasterizationSamples = mPipelineConfig.multisampleState.rasterizetionSample;
		multisampleStateInfo.sampleShadingEnable = mPipelineConfig.multisampleState.sampleShadingEnable;
		multisampleStateInfo.minSampleShading = mPipelineConfig.multisampleState.minSampleShading;
		multisampleStateInfo.pSampleMask = nullptr;
		multisampleStateInfo.alphaToCoverageEnable = VK_FALSE;
		multisampleStateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo = {};
		depthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilStateInfo.pNext = nullptr;
		depthStencilStateInfo.flags = 0;
		depthStencilStateInfo.depthTestEnable = mPipelineConfig.depthStencilState.depthTestEnable;
		depthStencilStateInfo.depthWriteEnable = mPipelineConfig.depthStencilState.depthWriteEnable;
		depthStencilStateInfo.depthCompareOp = mPipelineConfig.depthStencilState.depthCompareOp;
		depthStencilStateInfo.depthBoundsTestEnable = mPipelineConfig.depthStencilState.depthBoundstextEnable;
		depthStencilStateInfo.stencilTestEnable = mPipelineConfig.depthStencilState.stencilTestEnable;
		depthStencilStateInfo.front = {};
		depthStencilStateInfo.back = {};
		depthStencilStateInfo.minDepthBounds = 0.0f;
		depthStencilStateInfo.maxDepthBounds = 1.0f;

		VkPipelineColorBlendStateCreateInfo colorBlendStateInfo = {};
		colorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateInfo.pNext = nullptr;
		colorBlendStateInfo.flags = 0;
		colorBlendStateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateInfo.logicOp = VK_LOGIC_OP_CLEAR;
		colorBlendStateInfo.attachmentCount = 1;
		colorBlendStateInfo.pAttachments = &mPipelineConfig.colorBlendAttachmentState;
		colorBlendStateInfo.blendConstants[0] = colorBlendStateInfo.blendConstants[1] =colorBlendStateInfo.blendConstants[2]=colorBlendStateInfo.blendConstants[3] = 0;

		VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.pNext = nullptr;
		dynamicStateInfo.flags = 0;
		dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(mPipelineConfig.dynamicState.dynamicStates.size());
		if (dynamicStateInfo.dynamicStateCount > 0) {
			dynamicStateInfo.pDynamicStates = mPipelineConfig.dynamicState.dynamicStates.data();  // 将指针设置为动态状态数组
		} else {
			dynamicStateInfo.pDynamicStates = nullptr;  // 如果没有动态状态，设置为 nullptr
		}


		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = nullptr;
		pipelineInfo.flags = 0;
		pipelineInfo.stageCount = ARRAY_SIZE(shaderStageInfo);
		pipelineInfo.pStages = shaderStageInfo;
		pipelineInfo.pVertexInputState = &vertexInputStageInfo;
		pipelineInfo.pInputAssemblyState = &inputAssemblyStageInfo;
		pipelineInfo.pTessellationState = nullptr;
		pipelineInfo.pViewportState = &viewportStateInfo;
		pipelineInfo.pRasterizationState = &rasterizerStateInfo;
		pipelineInfo.pMultisampleState = &multisampleStateInfo;
		pipelineInfo.pDepthStencilState = &depthStencilStateInfo;
		pipelineInfo.pColorBlendState = &colorBlendStateInfo;
		pipelineInfo.pDynamicState = &dynamicStateInfo;
		pipelineInfo.renderPass = mRenderPass->GetHandle();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = 0;
		pipelineInfo.layout = mPipelineLayout->GetHandle();

		CALL_VK(vkCreateGraphicsPipelines(mDevice->GetHandle(),mDevice->GetPipelineCache(),1,&pipelineInfo,nullptr,&mHandle));

		LOG_I("Pipeline: {0}",(void*)mHandle);
	}

	LSVKPipeline *LSVKPipeline::SetVertexInputState(const std::vector<VkVertexInputBindingDescription> &vertexBindings,
													const std::vector<VkVertexInputAttributeDescription> &vertexAttris) {
		mPipelineConfig.vertexInputState.vertexBindingDescriptions = vertexBindings;
		mPipelineConfig.vertexInputState.vertexAttributes = vertexAttris;
		return this;
	}

	LSVKPipeline *LSVKPipeline::SetInputAssemblyState(VkPrimitiveTopology topology, VkBool32 primitiveRestarEnable) {
		mPipelineConfig.inputAssemblyState.topology = topology;
		mPipelineConfig.inputAssemblyState.primitiveRestartEnable = primitiveRestarEnable;
		return this;
	}

	LSVKPipeline *LSVKPipeline::SetRasterzationState(const PipelineRasterizationState &rasterzationState) {
		mPipelineConfig.rasterizationState.depthClampEnable = rasterzationState.depthClampEnable;
		mPipelineConfig.rasterizationState.rasterizerDiscardEnable = rasterzationState.rasterizerDiscardEnable;
		mPipelineConfig.rasterizationState.polygonMode = rasterzationState.polygonMode;
		mPipelineConfig.rasterizationState.cullMode = rasterzationState.cullMode;
		mPipelineConfig.rasterizationState.frontFace = rasterzationState.frontFace;
		mPipelineConfig.rasterizationState.depthBiasEnable = rasterzationState.depthBiasEnable;
		mPipelineConfig.rasterizationState.depthBiasConstantFactor = rasterzationState.depthBiasConstantFactor;
		mPipelineConfig.rasterizationState.depthBiasClamp = rasterzationState.depthBiasClamp;
		mPipelineConfig.rasterizationState.depthBiasSlopeFactor = rasterzationState.depthBiasSlopeFactor;
		mPipelineConfig.rasterizationState.lineWidth = rasterzationState.lineWidth;
		return this;
	}

	LSVKPipeline *LSVKPipeline::SetMultisampleState(VkSampleCountFlagBits samples, VkBool32 sampleShadingEnable, float minSampleShading) {
		mPipelineConfig.multisampleState.rasterizetionSample = samples;
		mPipelineConfig.multisampleState.sampleShadingEnable = sampleShadingEnable;
		mPipelineConfig.multisampleState.minSampleShading = minSampleShading;
		return this;
	}

	LSVKPipeline *LSVKPipeline::SetDepthStencilState(const PipelineDepthStencilState &depthStencilState) {
		mPipelineConfig.depthStencilState.depthTestEnable = depthStencilState.depthTestEnable;
		mPipelineConfig.depthStencilState.depthWriteEnable = depthStencilState.depthWriteEnable;
		mPipelineConfig.depthStencilState.depthCompareOp = depthStencilState.depthCompareOp;
		mPipelineConfig.depthStencilState.stencilTestEnable = depthStencilState.stencilTestEnable;
		mPipelineConfig.depthStencilState.stencilTestEnable = depthStencilState.stencilTestEnable;
		return this;
	}

	LSVKPipeline *LSVKPipeline::SetColorBlendAttachmentState(VkBool32 blendEnable,VkBlendFactor srcColorBlendFactor,
															VkBlendFactor dstColorBlendFactor,VkBlendOp colorBlendOp,
															VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor,
															VkBlendOp alphaBlendOp) {
		mPipelineConfig.colorBlendAttachmentState.blendEnable = blendEnable;
		mPipelineConfig.colorBlendAttachmentState.srcColorBlendFactor = srcColorBlendFactor;
		mPipelineConfig.colorBlendAttachmentState.dstColorBlendFactor = dstColorBlendFactor;
		mPipelineConfig.colorBlendAttachmentState.srcAlphaBlendFactor = srcAlphaBlendFactor;
		mPipelineConfig.colorBlendAttachmentState.dstAlphaBlendFactor = dstAlphaBlendFactor;
		mPipelineConfig.colorBlendAttachmentState.colorBlendOp = colorBlendOp;
		mPipelineConfig.colorBlendAttachmentState.alphaBlendOp = alphaBlendOp;
		return this;
	}

	LSVKPipeline *LSVKPipeline::SetDynamicState(const std::vector<VkDynamicState> &dynamicStates) {
		mPipelineConfig.dynamicState.dynamicStates=dynamicStates;
		return this;
	}

	LSVKPipeline *LSVKPipeline::EnableAlphaBlend() {
		mPipelineConfig.colorBlendAttachmentState = {
			.blendEnable = VK_TRUE,
			.srcColorBlendFactor=VK_BLEND_FACTOR_SRC_ALPHA,
			.dstColorBlendFactor=VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.colorBlendOp=VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor=VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.dstAlphaBlendFactor=VK_BLEND_FACTOR_ZERO,
			.alphaBlendOp=VK_BLEND_OP_ADD
		};
		return this;
	};

	LSVKPipeline *LSVKPipeline::EnableDepthTest() {
		mPipelineConfig.depthStencilState.depthTestEnable = VK_TRUE;
		mPipelineConfig.depthStencilState.depthWriteEnable = VK_TRUE;
		mPipelineConfig.depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
		return this;
	}

	void LSVKPipeline::Bind(VkCommandBuffer cmdBuffer) {
		vkCmdBindPipeline(cmdBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,mHandle);
	}


}