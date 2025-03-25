#include"Graphic/LSVKFramebuffer.h"
#include"Graphic/LSVKImageView.h"
#include"Graphic/LSVKDevice.h"
#include"Graphic/LSVKRenderPass.h"
#include"Graphic/LSVKImage.h"

namespace lse{
	LSVKFramebuffer::LSVKFramebuffer(LSVKDevice* device, LSVKRenderPass* renderPass, const std::vector<std::shared_ptr<LSVKImage>>&images, uint32_t width, uint32_t height)
	: mDevice(device),mRenderPass(renderPass),mImages(images),mWidth(width),mHeight(height),mHandle(VK_NULL_HANDLE) {
		Recreate(images, width, height);
	}

	LSVKFramebuffer::~LSVKFramebuffer() {
		if (mHandle != VK_NULL_HANDLE) {
			LOG_T("Destroying Framebuffer: {0}", (void*)mHandle);
			vkDestroyFramebuffer(mDevice->GetHandle(), mHandle, nullptr);
			mHandle = VK_NULL_HANDLE;
		}
	}

	bool LSVKFramebuffer::Recreate(const std::vector<std::shared_ptr<LSVKImage>>&images, uint32_t width, uint32_t height) {
		VkResult ret;

		if (!mDevice || !mRenderPass) {
			LOG_E("Invalid device or render pass for framebuffer recreation.");
			return false;
		}

		VK_D(Framebuffer,mDevice->GetHandle(),mHandle);

		mWidth = width;
		mHeight = height;

		//先把framebuffer内部的图像视图清空
		mImageViews.clear();

		std::vector<VkImageView>attachments(images.size());
		for (int i = 0; i < images.size();i++) {
			bool isDepthFormat  =  IsDepthStencilFormat(images[i]->GetFormat());
			mImageViews.push_back(std::make_shared<LSVKImageView>(mDevice, images[i]->GetHandle(),
									images[i]->GetFormat(), isDepthFormat ? VK_IMAGE_ASPECT_DEPTH_BIT :VK_IMAGE_ASPECT_COLOR_BIT));
			attachments[i] = mImageViews[i]->GetHandle();
		}

		VkFramebufferCreateInfo frameBufferInfo = {};
		frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferInfo.pNext = nullptr;
		frameBufferInfo.flags = 0;
		frameBufferInfo.renderPass = mRenderPass->GetHandle();
		frameBufferInfo.attachmentCount = static_cast<uint32_t>(mImageViews.size());
		frameBufferInfo.pAttachments = attachments.data();
		frameBufferInfo.width = width;
		frameBufferInfo.height = height;
		frameBufferInfo.layers = 1;

		ret = vkCreateFramebuffer(mDevice->GetHandle(), &frameBufferInfo, nullptr, &mHandle);
		if (ret != VK_SUCCESS) {
			LOG_E("Failed to create framebuffer. Error code: {0}", vk_result_string(ret));
			return false;
		}
		LOG_T("FrameBuffer: {0}, new: {1}, width: {2}, height: {3}, view count: {4}", __FUNCTION__, (void*)mHandle, mWidth, mHeight, mImageViews.size());
		
		return ret == VK_SUCCESS;
	}

}