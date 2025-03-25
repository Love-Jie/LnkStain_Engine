#include"Graphic/LsVKImageView.h"
#include"Graphic/LSVKDevice.h"

namespace lse {
	LSVKImageView::LSVKImageView(LSVKDevice* device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) : mDevice(device) {
		VkImageViewCreateInfo imageViewInfo = {};
		imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewInfo.pNext = nullptr;
		imageViewInfo.image = image;
        imageViewInfo.flags = 0;
		imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewInfo.format = format;
		imageViewInfo.components = {
			VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY
		};
		imageViewInfo.subresourceRange.aspectMask = aspectFlags;
		imageViewInfo.subresourceRange.baseMipLevel = 0;
		imageViewInfo.subresourceRange.levelCount = 1;
		imageViewInfo.subresourceRange.baseArrayLayer = 0;
		imageViewInfo.subresourceRange.layerCount = 1;

		CALL_VK(vkCreateImageView(device->GetHandle(), &imageViewInfo, nullptr, &mHandle));
	}

	LSVKImageView::~LSVKImageView() {
		VK_D(ImageView,mDevice->GetHandle(),mHandle);
	}

}