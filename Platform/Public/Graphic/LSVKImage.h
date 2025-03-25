#pragma once

#include"Graphic/LSVKCommon.h"

namespace lse {
    class LSVKDevice;
    class LSVKBuffer;
    class LSVKImage {
    public:
        LSVKImage(LSVKDevice *device, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);
        LSVKImage(LSVKDevice *device, VkImage image, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);
        ~LSVKImage();

        static bool TransitionLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
        void CopyFromBuffer(VkCommandBuffer cmdBuffer,LSVKBuffer *buffer);

        VkFormat GetFormat() const {return mFormat;};
        VkImage GetHandle() const {return mHandle;};
    private:
        bool bCreateImage = true;

        VkImage mHandle = VK_NULL_HANDLE;
        VkDeviceMemory mMemory = VK_NULL_HANDLE;
        VkFormat mFormat;
        LSVKDevice *mDevice;

        VkExtent3D mExtent;
        VkImageUsageFlags mUsage;
    };

}

