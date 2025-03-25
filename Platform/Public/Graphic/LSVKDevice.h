#pragma once
#include"LsVKCommon.h"
#include"Graphic/LSVKQueue.h"
#include"Graphic/LSVKCommandBuffer.h"

namespace lse {
	class LSVKGraphicContext;
	class LSVKQueue;
	class LSVKCommandPool;

	//窗口表面格式设置
	struct LsVkSettings {
		VkFormat surfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
		VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
		uint32_t swapchainImageCount = 3;   //交换链内部图片数量
	};

	class LSVKDevice {
	public:
		LSVKDevice(LSVKGraphicContext* context, uint32_t graphicQueueCount, uint32_t presentQueueCount, const LsVkSettings& settings = {});

		~LSVKDevice();

		VkDevice GetHandle() const { return mHandle; };
		const LsVkSettings &GetSettings() const { return mSettings; };
		VkPipelineCache GetPipelineCache() const { return mPipelineCache; };

		LSVKQueue *GetGraphicQueue(uint32_t index) const { return mGraphicQueues.size() < index + 1 ? nullptr : mGraphicQueues[index].get(); };
		LSVKQueue *GetFirstGraphicQueue() const { return mGraphicQueues.empty() ? nullptr : mGraphicQueues[0].get(); };
		LSVKQueue *GetPresentQueue(uint32_t index) const { return mPresentQueues.size() < index + 1 ? nullptr : mPresentQueues[index].get(); };
		LSVKQueue *GetFirstPresentQueue() const { return mPresentQueues.empty() ? nullptr : mPresentQueues[0].get(); };
		LSVKCommandPool *GetDefaultCmdPool() const { return mDefaultCmdPool.get(); };

		int32_t GetMemoryIndex(VkMemoryPropertyFlags memProps,uint32_t memoryTypeBits) const;
		VkCommandBuffer CreateAndBeginOneCmdBuffer();
		void SubmitOneCmdBuffer(VkCommandBuffer cmdBuffer);

		VkResult CreateSimpleSampler(VkFilter filter, VkSamplerAddressMode addressMode, VkSampler *outSampler);
	private:
		void CreatePipelineCache();
		void CreateDefaultCmdPool();

		VkDevice mHandle = VK_NULL_HANDLE;
		LSVKGraphicContext *mContext;
		std::vector<std::shared_ptr<LSVKQueue>>mGraphicQueues;
		std::vector<std::shared_ptr<LSVKQueue>>mPresentQueues;
		std::shared_ptr<LSVKCommandPool> mDefaultCmdPool;
		LsVkSettings mSettings;

		VkPipelineCache mPipelineCache = VK_NULL_HANDLE;
	};

	
}