#pragma once
#include"LSVKCommon.h"
#include"Graphic/LSVKdevice.h"

namespace lse {
	class LSVKDevice;
	class LSVKImageView {
	public:
		LSVKImageView(LSVKDevice *device,VkImage image,VkFormat format, VkImageAspectFlags aspectFlags);
		~LSVKImageView();

		VkImageView GetHandle() const { return mHandle; };
	private:
		VkImageView mHandle = VK_NULL_HANDLE;
		LSVKDevice* mDevice;
	};
	
}