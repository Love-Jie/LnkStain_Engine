#include"LSVKSwapchain.h"
#include"LSVKGraphicContext.h"
#include"LSVKDevice.h"

namespace lse {
	LSVKSwapchain::LSVKSwapchain(LSVKGraphicContext* context, LSVKDevice* device):mContext(context),mDevice(device) {
		ReCreate();
	}

	LSVKSwapchain::~LSVKSwapchain() {

		vkDeviceWaitIdle(mDevice->GetHandle());
		VK_D(SwapchainKHR,mDevice->GetHandle(),mHandle);
	}

	bool LSVKSwapchain::ReCreate() {
		LOG_D("-----------------------");
		SetupSurfaceCapabilities();

		LOG_D("currentExtent : {0} x {1}", mSurfaceInfo.capabilities.currentExtent.width, mSurfaceInfo.capabilities.currentExtent.height);
		LOG_D("surfaceFormat : {0}", vk_format_string(mSurfaceInfo.surfaceFormat.format));
		LOG_D("presentMode   : {0}", vk_present_mode_string(mSurfaceInfo.presentMode));
		LOG_D("-----------------------");

		uint32_t imageCount = mDevice->GetSettings().swapchainImageCount;
		if (imageCount < mSurfaceInfo.capabilities.minImageCount && mSurfaceInfo.capabilities.minImageCount >0) {
			imageCount = mSurfaceInfo.capabilities.minImageCount;
		}
		if (imageCount > mSurfaceInfo.capabilities.maxImageCount && mSurfaceInfo.capabilities.maxImageCount >0) {
			imageCount = mSurfaceInfo.capabilities.maxImageCount;
		}

		VkSharingMode imageSharingMode;
		uint32_t queueFamilyIndexCount;
		uint32_t pQueueFamilyIndices[2] = { 0,0 };
		if (mContext->IsSameGraphicPresentQueueFamily()) {
			imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			queueFamilyIndexCount = 0;
		}
		else {
			imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			queueFamilyIndexCount = 2;
			pQueueFamilyIndices[0] = mContext->GetGraphicQueueFamilyInfo().queueFamilyIndex;
			pQueueFamilyIndices[1] = mContext->GetPresentQueueFamilyInfo().queueFamilyIndex;
		}

		//原来的交换链
		VkSwapchainKHR oldSwapchain = mHandle;

		VkSwapchainCreateInfoKHR swapchainInfo = {};
		swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainInfo.pNext = nullptr;
		swapchainInfo.flags = 0;
		swapchainInfo.surface = mContext->GetSurface();
		swapchainInfo.minImageCount = imageCount;
		swapchainInfo.imageFormat = mSurfaceInfo.surfaceFormat.format;
		swapchainInfo.presentMode = mSurfaceInfo.presentMode;
		swapchainInfo.imageColorSpace = mSurfaceInfo.surfaceFormat.colorSpace;
		swapchainInfo.imageExtent = mSurfaceInfo.capabilities.currentExtent;
		swapchainInfo.imageArrayLayers = 1;
		swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainInfo.imageSharingMode = imageSharingMode;
		swapchainInfo.queueFamilyIndexCount = queueFamilyIndexCount;
		swapchainInfo.pQueueFamilyIndices = pQueueFamilyIndices;
		swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		swapchainInfo.oldSwapchain = oldSwapchain;
		swapchainInfo.presentMode = mSurfaceInfo.presentMode;
		swapchainInfo.clipped = VK_FALSE;
		
		// 设置compositeAlpha ֵ
		if (mSurfaceInfo.capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
			swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		}
		else if (mSurfaceInfo.capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
			swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
		}
		else if (mSurfaceInfo.capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
			swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
		}
		else {
			swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
		}
		
		if (vkCreateSwapchainKHR(mDevice->GetHandle(), &swapchainInfo, nullptr, &mHandle) != VK_SUCCESS){
			LOG_E("{0}: failed to create a swapchain", __FUNCTION__);
			return false;
		}
		LOG_T("Swapchain {0} : old :{1},new :{2},image cout : {3}, format : {4},present mode : {5}", __FUNCTION__, (void*)oldSwapchain, (void*)mHandle, imageCount, vk_format_string(mSurfaceInfo.surfaceFormat.format),vk_present_mode_string(mSurfaceInfo.presentMode));
		
		// 必须在创建成功后，再销毁旧的交换链，否则在后续资源销毁的时候显示device比swapchain早销毁，交换链销毁不完全
		if (oldSwapchain != VK_NULL_HANDLE) {
			vkDeviceWaitIdle(mDevice->GetHandle());
			vkDestroySwapchainKHR(mDevice->GetHandle(), oldSwapchain, nullptr);
		}  

		uint32_t swapchainImageCount;
		VkResult ret;
		ret = vkGetSwapchainImagesKHR(mDevice->GetHandle(), mHandle, &swapchainImageCount, nullptr);
		mImages.resize(swapchainImageCount);
		ret = vkGetSwapchainImagesKHR(mDevice->GetHandle(), mHandle, &swapchainImageCount, mImages.data());
		return ret==VK_SUCCESS;
	}

	void LSVKSwapchain::SetupSurfaceCapabilities() {
		//capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mContext->GetPhysicalDevice(), mContext->GetSurface(), &mSurfaceInfo.capabilities);
		
		LsVkSettings settings = mDevice->GetSettings();

		//format
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(mContext->GetPhysicalDevice(), mContext->GetSurface(), &formatCount, nullptr);
		if (formatCount == 0){
			LOG_E("{0}: num of surface format is 0.", __FUNCTION__);
			return;
		}

		std::vector<VkSurfaceFormatKHR>formats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(mContext->GetPhysicalDevice(), mContext->GetSurface(), &formatCount, formats.data());
		

		int32_t foundFormatIndex = -1;
		for (uint32_t i = 0; i < formatCount; i++) {
			if (formats[i].format == settings.surfaceFormat) {
				foundFormatIndex = i;
				break;
			}
		}
		if (foundFormatIndex == -1) {
			foundFormatIndex = 0;
		}

		mSurfaceInfo.surfaceFormat = formats[foundFormatIndex];

		//presentMode
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(mContext->GetPhysicalDevice(), mContext->GetSurface(), &presentModeCount, nullptr);
		if (presentModeCount == 0) {
			LOG_E("{0}: num of surface present mode is 0.", __FUNCTION__);
			return;
		}

		std::vector<VkPresentModeKHR>presentModes(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(mContext->GetPhysicalDevice(), mContext->GetSurface(), &presentModeCount, presentModes.data());
	
		VkPresentModeKHR perferredPresentMode = mDevice->GetSettings().presentMode;
		int32_t foundPresentModeIndex = -1;
		for (uint32_t i = 0; i < presentModeCount; i++) {
			if (presentModes[i] == perferredPresentMode) {
				foundPresentModeIndex = i;
				break;
			}
		}
		if (foundPresentModeIndex == -1) {
			mSurfaceInfo.presentMode = presentModes[foundFormatIndex];
		}
		else {
			mSurfaceInfo.presentMode = presentModes[0];
		}
	}

	VkResult LSVKSwapchain::AcquireImage(int32_t *outImageIndex,VkSemaphore semaphore,VkFence fence)  {
		uint32_t imageIndex;
		VkResult ret = vkAcquireNextImageKHR(mDevice->GetHandle(), mHandle, UINT64_MAX, semaphore, fence, &imageIndex);
		if(fence != VK_NULL_HANDLE){
			CALL_VK(vkWaitForFences(mDevice->GetHandle(), 1, &fence, VK_FALSE, UINT64_MAX));
			CALL_VK(vkResetFences(mDevice->GetHandle(), 1, &fence));
		}

		if(ret == VK_SUCCESS || ret == VK_SUBOPTIMAL_KHR){
			*outImageIndex = imageIndex;
			mCurrentImageIndex = imageIndex;
		}
		return ret;
	}


	VkResult LSVKSwapchain::Present(int32_t imageIndex,const std::vector<VkSemaphore> &waitSemaphores)  {
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
		presentInfo.pWaitSemaphores = waitSemaphores.data();
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &mHandle;
		presentInfo.pImageIndices = reinterpret_cast<const uint32_t *>(&imageIndex);

		VkResult ret = vkQueuePresentKHR(mDevice->GetFirstPresentQueue()->GetHandle(), &presentInfo);
		mDevice->GetFirstPresentQueue()->WaitIdle();
		return ret;
	}


}