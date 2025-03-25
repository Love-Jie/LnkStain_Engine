#include"LsVKQueue.h"

namespace lse{
	LSVKQueue::LSVKQueue(uint32_t familyIndex, uint32_t index, VkQueue queue, bool canPresent)
		:mFamilyIndex(familyIndex), mIndex(index), mHandle(queue), canPresent(canPresent) {
		LOG_T("Create a new queue: {0} - {1} - {2}, present: {3}", mFamilyIndex, index, (void*)queue, canPresent);
	}

	void LSVKQueue::WaitIdle() const {
		CALL_VK(vkQueueWaitIdle(mHandle));
	}


	void LSVKQueue::submit(std::vector<VkCommandBuffer> cmdBuffers, const std::vector<VkSemaphore> &waitSemaphores, const std::vector<VkSemaphore> &signalSemaphores,VkFence frameFence) {
		VkPipelineStageFlags waitDstStageMask[] = {VK_PIPELINE_STAGE_ALL_COMMANDS_BIT};
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
		submitInfo.pWaitSemaphores = waitSemaphores.data();
		submitInfo.pWaitDstStageMask = waitDstStageMask;
		submitInfo.commandBufferCount = static_cast<uint32_t>(cmdBuffers.size());
		submitInfo.pCommandBuffers = cmdBuffers.data();
		submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
		submitInfo.pSignalSemaphores =signalSemaphores.data();
		CALL_VK(vkQueueSubmit(mHandle,1,&submitInfo,frameFence));
	}


}