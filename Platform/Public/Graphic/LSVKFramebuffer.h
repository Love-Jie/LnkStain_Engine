#pragma once
#include"Graphic/LSVKCommon.h"
#include"Graphic/LSVKImageView.h"

namespace lse {
	class LSVKDevice;
	class LSVKRenderPass;
	class LSVKImageview;
	class LSVKImage;
	
	class LSVKFramebuffer {
	public:
		LSVKFramebuffer(LSVKDevice *device,LSVKRenderPass *renderPass,const std::vector<std::shared_ptr<LSVKImage>>&images,uint32_t width,uint32_t height);
		~LSVKFramebuffer();

		bool Recreate(const std::vector<std::shared_ptr<LSVKImage>>&images, uint32_t width, uint32_t height);  //当窗口发生变化时需要进行帧缓冲的重建

		VkFramebuffer GetHandle()const {return mHandle;};
		uint32_t GetWidth() const {return mWidth;};
		uint32_t GetHeight() const {return mHeight;};

	private:
		VkFramebuffer mHandle;
		LSVKDevice* mDevice;
		LSVKRenderPass* mRenderPass;
		uint32_t mWidth;
		uint32_t mHeight;
		std::vector<std::shared_ptr<LSVKImage>> mImages;
		std::vector<std::shared_ptr<LSVKImageView>> mImageViews;


	};
}