#pragma once

#include"LSVKCommon.h"

namespace lse{
    class LSVKImage;
    class LSVKImageView;
    class LSVKBuffer;

    struct RGBColor {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    class LSTexture{
    public:
        LSTexture(const std::string&filePath);
        LSTexture(uint32_t width, uint32_t height, RGBColor *pixels);
        ~LSTexture();

        uint32_t GetWidth() const { return mWidth; }
        uint32_t GetHeight() const { return mHeight; }
        LSVKImage *GetImage() const { return mImage.get(); }
        LSVKImageView *GetImageView() const { return mImageView.get(); }

    private:
        void CreateImage(size_t size, void *data);

        uint32_t mWidth;
        uint32_t mHeight;
        VkFormat mFormat;
        std::shared_ptr<LSVKImage> mImage;
        std::shared_ptr<LSVKImageView> mImageView;

    };

}