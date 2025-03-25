#include"LSVKDevice.h"
#include"LSVKGraphicContext.h"
#include"LSVKQueue.h"

namespace lse {
	//交换链扩展
	const DeviceFeature requestedExtensions[] = {
		{VK_KHR_SWAPCHAIN_EXTENSION_NAME,true},
#ifdef LS_ENGINE_PLATFORM_WIN32
		//{"VK_KHR_portability_subset",true}
#elif LS_ENGINE_PLATFORM_MACOS
//Macos
#elif LS_ENGINE_PLATFORM_LINUX
// Linux
#endif

	};

	LSVKDevice::LSVKDevice(LSVKGraphicContext* context, uint32_t graphicQueueCount, uint32_t presentQueueCount, const LsVkSettings& settings):mContext(context),mSettings(settings) {
		if (!context) {
			LOG_E("Must create a vulkan graphic context before create device.");
			return;
		}

		QueueFamilyInfo graphicQueueFamilyInfo = context->GetGraphicQueueFamilyInfo();
		QueueFamilyInfo presentQueueFamilyInfo = context->GetPresentQueueFamilyInfo();

		//先做判断，判断队列是否正确
		if (graphicQueueCount > graphicQueueFamilyInfo.queueCount) {
			LOG_E("this queue family has {0} queue, but request {1}.",graphicQueueFamilyInfo.queueCount, graphicQueueCount);
			return;
		}
		if (presentQueueCount > presentQueueFamilyInfo.queueCount) {
			LOG_E("this queue family has {0} queue, but request {1}.", presentQueueFamilyInfo.queueCount, presentQueueCount);
			return;
		}

		//设置队列优先级，显示队列优先级要高于图形队列优先级
		std::vector<float>graphicQueuePriorities(graphicQueueCount, 0.0f);
		std::vector<float>presentQueuePriorities(presentQueueCount, 1.0f);

		//是否为同一队列族
		bool bSameQueueFamilyIndex = context->IsSameGraphicPresentQueueFamily();
		uint32_t sameQueueCount = graphicQueueCount;
		if (bSameQueueFamilyIndex) {
			sameQueueCount += presentQueueCount;
			if (sameQueueCount > graphicQueueFamilyInfo.queueCount) {
				sameQueueCount = graphicQueueFamilyInfo.queueCount;
			}
			graphicQueuePriorities.insert(graphicQueuePriorities.end(), presentQueuePriorities.begin(), presentQueuePriorities.end());
		}

		//设备队列族创建信息
		VkDeviceQueueCreateInfo queueInfos[2] = {};
		queueInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfos[0].pNext = nullptr;
		queueInfos[0].flags = 0;
		queueInfos[0].queueFamilyIndex = graphicQueueFamilyInfo.queueFamilyIndex;
		queueInfos[0].queueCount = sameQueueCount;
		queueInfos[0].pQueuePriorities = graphicQueuePriorities.data();

		if (!bSameQueueFamilyIndex) {
			queueInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfos[1].pNext = nullptr;
			queueInfos[1].flags = 0;
			queueInfos[1].queueFamilyIndex = presentQueueFamilyInfo.queueFamilyIndex;
			queueInfos[1].queueCount = presentQueueCount;
			queueInfos[1].pQueuePriorities = presentQueuePriorities.data();
		}

		//检测拓展是否可用
		uint32_t availableExtensionCount;
		vkEnumerateDeviceExtensionProperties(context->GetPhysicalDevice(), "", &availableExtensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
		vkEnumerateDeviceExtensionProperties(context->GetPhysicalDevice(), "", &availableExtensionCount, availableExtensions.data());

		uint32_t enableExtensionCount;
		const char* enableExtensions[32];

		if (!checkDeviceFeatures("Device Extensions", true, availableExtensionCount, availableExtensions.data(), ARRAY_SIZE(requestedExtensions),requestedExtensions, &enableExtensionCount, enableExtensions)) {
			return;
		}

		VkDeviceCreateInfo deviceInfo = {};
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.pNext = nullptr;
		deviceInfo.flags = 0;
		deviceInfo.queueCreateInfoCount = bSameQueueFamilyIndex ? 1 : 2;
		deviceInfo.pQueueCreateInfos = queueInfos;
		deviceInfo.enabledLayerCount = 0;
		deviceInfo.ppEnabledLayerNames = nullptr;
		deviceInfo.enabledExtensionCount = enableExtensionCount;
		deviceInfo.ppEnabledExtensionNames = enableExtensionCount > 0 ? enableExtensions : nullptr;
		deviceInfo.pEnabledFeatures = nullptr;


		CALL_VK(vkCreateDevice(context->GetPhysicalDevice(),&deviceInfo,nullptr,&mHandle));
		LOG_T("VKDevice: {0}", (void*)mHandle);

		for (uint32_t i = 0; i < graphicQueueCount; i++) {
			VkQueue queue;
			vkGetDeviceQueue(mHandle, graphicQueueFamilyInfo.queueFamilyIndex, i, &queue);
			mGraphicQueues.push_back(std::make_shared<LSVKQueue>(graphicQueueFamilyInfo.queueFamilyIndex, i, queue, false));

		}
		for (uint32_t i = 0; i < presentQueueCount; i++) {
			VkQueue queue;
			vkGetDeviceQueue(mHandle, presentQueueFamilyInfo.queueFamilyIndex, i, &queue);
			mPresentQueues.push_back(std::make_shared<LSVKQueue>(presentQueueFamilyInfo.queueFamilyIndex, i, queue, true));

		}

		//create a pipeline cache
		CreatePipelineCache();

		//create default command pool
		CreateDefaultCmdPool();

	}
	LSVKDevice::~LSVKDevice() {
		LOG_I("destroy device");
		mDefaultCmdPool = nullptr;
		//vkDeviceWaitIdle(mHandle);
		vkDestroyPipelineCache(mHandle,mPipelineCache,nullptr);
		vkDestroyDevice(mHandle,nullptr);
	
	}

	 void LSVKDevice::CreatePipelineCache() {
		VkPipelineCacheCreateInfo pipelineCacheInfo = {};
		pipelineCacheInfo.flags = 0;
		pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		pipelineCacheInfo.pNext = nullptr;

		CALL_VK(vkCreatePipelineCache(mHandle,&pipelineCacheInfo,nullptr,&mPipelineCache));
	 }

	void LSVKDevice::CreateDefaultCmdPool() {
		mDefaultCmdPool = std::make_shared<lse::LSVKCommandPool>(this,mContext->GetGraphicQueueFamilyInfo().queueFamilyIndex);
	}


	int32_t LSVKDevice::GetMemoryIndex(VkMemoryPropertyFlags memProps, uint32_t memoryTypeBits) const {
		VkPhysicalDeviceMemoryProperties phyDeviceMemProps = mContext->GetPhysicalDeviceMemoryProperties();
		if(phyDeviceMemProps.memoryTypeCount == 0) {
			LOG_E("Physical device memory type count is 0");
			return -1;
		}
		for(int i = 0; i < phyDeviceMemProps.memoryTypeCount; i++) {
			if(memoryTypeBits & (1 << i) && (phyDeviceMemProps.memoryTypes[i].propertyFlags & memProps) == memProps) {
				return i;
			}
		}
		LOG_E("Can not find memory type index : type bit: {0}",memoryTypeBits);
		return 0;
	}

	VkCommandBuffer LSVKDevice::CreateAndBeginOneCmdBuffer() {
		VkCommandBuffer cmdBuffer = mDefaultCmdPool->AllocateOneCommandBuffer();
		mDefaultCmdPool->BeginCommandBuffer(cmdBuffer);
		return cmdBuffer;
	}

	void LSVKDevice::SubmitOneCmdBuffer(VkCommandBuffer cmdBuffer) {
		mDefaultCmdPool->EndCommandBuffer(cmdBuffer);
		LSVKQueue *queue = GetFirstGraphicQueue();
		queue->submit({cmdBuffer});
		queue->WaitIdle();
	}

	VkResult LSVKDevice::CreateSimpleSampler(VkFilter filter, VkSamplerAddressMode addressMode, VkSampler *outSampler) {
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo .pNext = nullptr;
		samplerInfo .flags = 0;
		samplerInfo .magFilter = filter;
		samplerInfo .minFilter = filter;
		samplerInfo .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo .addressModeU = addressMode;
		samplerInfo .addressModeV = addressMode;
		samplerInfo .addressModeW = addressMode;
		samplerInfo .mipLodBias = 0;
		samplerInfo .anisotropyEnable = VK_FALSE;
		samplerInfo .maxAnisotropy = 0;
		samplerInfo .compareEnable = VK_FALSE;
		samplerInfo .compareOp = VK_COMPARE_OP_NEVER;
		samplerInfo .minLod = 0;
		samplerInfo .maxLod = 1;
		samplerInfo .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		samplerInfo .unnormalizedCoordinates = VK_FALSE;

		return vkCreateSampler(mHandle, &samplerInfo, nullptr, outSampler);

	}

}