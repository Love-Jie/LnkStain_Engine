#include"Render/LSTexture.h"
#include"Graphic/LSVKImage.h"
#include"Graphic/LSVKImageView.h"
#include"Graphic/LSVKBuffer.h"
#include "LSApplication.h"
#include "Render/LSRenderContext.h"

#define STB_IMAGE_IMPLEMENTATION
#include"stb/stb_image.h"

namespace lse{
    LSTexture::LSTexture(const std::string &filePath) {
        int numChannels;    //图片有多少个通道（RGB）
        uint8_t *data = stbi_load(filePath.c_str(), reinterpret_cast<int *>(&mWidth), reinterpret_cast<int *>(&mHeight), &numChannels, STBI_rgb_alpha);
        if(!data){
            LOG_E("Can not load this image: {0}", filePath);
            return;
        }

        mFormat = VK_FORMAT_R8G8B8A8_UNORM;
        size_t size = sizeof(uint8_t) * 4 * mWidth * mHeight;
        CreateImage(size, data);
        stbi_image_free(data);
    }

    LSTexture::LSTexture(uint32_t width, uint32_t height, RGBColor *pixels) : mWidth(width), mHeight(height){
        mFormat = VK_FORMAT_R8G8B8A8_UNORM;
        size_t size = sizeof(uint8_t) * 4 * mWidth * mHeight;
        CreateImage(size, pixels);
    }

    LSTexture::~LSTexture() {
        mImageView.reset();
        mImage.reset();
    }

    void LSTexture::CreateImage(size_t size, void *data) {
        lse::LSRenderContext *renderContext = LSApplication::GetAppContext()->renderContext;
        lse::LSVKDevice *device = renderContext->GetDevice();
        mImage = std::make_shared<LSVKImage>(device, VkExtent3D{ mWidth, mHeight, 1}, mFormat, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT);
        mImageView = std::make_shared<LSVKImageView>(device, mImage->GetHandle(), mFormat, VK_IMAGE_ASPECT_COLOR_BIT);

        // copy data to buffer
        std::shared_ptr<LSVKBuffer> stageBuffer = std::make_shared<LSVKBuffer>(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, data, true);

        // UNDEFINED  -->  TRANSFER_DST --> copy --> SHADER_READ_ONLY_OPTIMAL

        // copy buffer to image
        VkCommandBuffer cmdBuffer = device->CreateAndBeginOneCmdBuffer();
        LSVKImage::TransitionLayout(cmdBuffer, mImage->GetHandle(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        mImage->CopyFromBuffer(cmdBuffer, stageBuffer.get());
        LSVKImage::TransitionLayout(cmdBuffer, mImage->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        device->SubmitOneCmdBuffer(cmdBuffer);

        stageBuffer.reset();
    }
}
