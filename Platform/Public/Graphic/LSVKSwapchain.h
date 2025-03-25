#pragma once
#include"LSVKCommon.h"

namespace lse {
	class LSVKGraphicContext;
	class LSVKDevice;


	struct SurfaceInfo {
		VkSurfaceCapabilitiesKHR capabilities;
		VkSurfaceFormatKHR surfaceFormat;
		VkPresentModeKHR presentMode;
	};

	class LSVKSwapchain {
	public:
		LSVKSwapchain(LSVKGraphicContext* context, LSVKDevice* device);
		~LSVKSwapchain();

		bool ReCreate();

		VkResult AcquireImage(int32_t *outImageIndex,VkSemaphore semaphore,VkFence fence = VK_NULL_HANDLE) ;

		VkResult Present(int32_t imageIndex,const std::vector<VkSemaphore> &waitSemaphores) ;

		const std::vector<VkImage>GetImages() const { return mImages; };
		uint32_t GetWidth() const { return mSurfaceInfo.capabilities.currentExtent.width; };
		uint32_t GetHeight() const { return mSurfaceInfo.capabilities.currentExtent.height; };
		int32_t GetCurrentImageIndex() const { return mCurrentImageIndex; };
		const SurfaceInfo &GetSurfaceInfo() const { return mSurfaceInfo; }
	private:
		void SetupSurfaceCapabilities();

		VkSwapchainKHR mHandle = VK_NULL_HANDLE;

		LSVKGraphicContext* mContext;
		LSVKDevice* mDevice;
		SurfaceInfo	mSurfaceInfo;
		std::vector<VkImage>mImages;

		int32_t mCurrentImageIndex = -1;
	};
	
}