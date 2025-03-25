#pragma once

#include"LSVKCommon.h"

namespace lse {
	class LSVKQueue {
	public:
		LSVKQueue(uint32_t familyIndex, uint32_t index, VkQueue queue, bool canPresent);
		~LSVKQueue() = default;

		void WaitIdle() const;
		void submit(std::vector<VkCommandBuffer>cmdBuffers,const std::vector<VkSemaphore> &waitSemaphores={}, const std::vector<VkSemaphore> &signalSemaphores={},VkFence frameFence = VK_NULL_HANDLE);

		VkQueue GetHandle() const { return mHandle; };
	private:
		uint32_t mFamilyIndex;
		uint32_t mIndex;
		VkQueue mHandle;
		bool canPresent;

	};
	
}