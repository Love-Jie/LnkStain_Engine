#include"Graphic/LSVKImage.h"
#include"Graphic/LSVKDevice.h"
#include"Graphic/LSVKBuffer.h"

namespace lse {
    LSVKImage::LSVKImage(LSVKDevice *device, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits sampleCount) : mDevice(device),mExtent(extent), mFormat(format), mUsage(usage){
        VkImageTiling tiling = VK_IMAGE_TILING_LINEAR;
        bool isDepthStencilFormat = IsDepthStencilFormat(format);
        if(isDepthStencilFormat) {
            tiling = VK_IMAGE_TILING_OPTIMAL;
        }

        VkImageCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.imageType = VK_IMAGE_TYPE_2D;
        createInfo.format = format;
        createInfo.extent = extent;
        createInfo.mipLevels = 1;
        createInfo.arrayLayers = 1;
        createInfo.samples = sampleCount;
        createInfo.tiling = tiling;
        createInfo.usage = usage;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
        createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        CALL_VK(vkCreateImage(device->GetHandle(),&createInfo,nullptr,&mHandle));

        //allocate memory
        VkMemoryRequirements memReqs;
        vkGetImageMemoryRequirements(mDevice->GetHandle(),mHandle,&memReqs);

        VkMemoryAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.pNext = nullptr;
        allocateInfo.allocationSize = memReqs.size;
        allocateInfo.memoryTypeIndex = static_cast<uint32_t>(mDevice->GetMemoryIndex(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,memReqs.memoryTypeBits));

        CALL_VK(vkAllocateMemory(mDevice->GetHandle(),&allocateInfo,nullptr, &mMemory));

        CALL_VK(vkBindImageMemory(mDevice->GetHandle(),mHandle,mMemory,0));

    }

    LSVKImage::LSVKImage(LSVKDevice *device, VkImage image, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits sampleCount)
                           : mHandle(image), mDevice(device), mExtent(extent), mFormat(format), mUsage(usage), bCreateImage(false) {
    }

    LSVKImage::~LSVKImage() {
        if(bCreateImage) {
            VK_D(Image,mDevice->GetHandle(),mHandle);
            VK_F(mDevice->GetHandle(),mMemory);
        }
    }

    void LSVKImage::CopyFromBuffer(VkCommandBuffer cmdBuffer, LSVKBuffer *buffer) {
        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = mExtent.width;
        region.bufferImageHeight = mExtent.height;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { mExtent.width, mExtent.height, 1 };

        vkCmdCopyBufferToImage(cmdBuffer, buffer->GetHandle(), mHandle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    }

    bool LSVKImage::TransitionLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
        if(image == VK_NULL_HANDLE){
            return false;
        }
        if(oldLayout == newLayout){
            return true;
        }
        VkImageMemoryBarrier barrier;
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = nullptr;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

        // Source layouts (old)
        // The source access mask controls actions to be finished on the old
        // layout before it will be transitioned to the new layout.
        switch (oldLayout) {
            case VK_IMAGE_LAYOUT_UNDEFINED:
                // Image layout is undefined (or does not matter).
                // Only valid as initial layout. No flags required.
                barrier.srcAccessMask = 0;
                break;

            case VK_IMAGE_LAYOUT_PREINITIALIZED:
                // Image is preinitialized.
                // Only valid as initial layout for linear images; preserves memory
                // contents. Make sure host writes have finished.
                barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                // Image is a color attachment.
                // Make sure writes to the color buffer have finished
                barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                // Image is a depth/stencil attachment.
                // Make sure any writes to the depth/stencil buffer have finished.
                barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                // Image is a transfer source.
                // Make sure any reads from the image have finished
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                // Image is a transfer destination.
                // Make sure any writes to the image have finished.
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                // Image is read by a shader.
                // Make sure any shader reads from the image have finished
                barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                break;
            default:
                LOG_E("Unsupported layout transition : {0} --> {1}", vk_image_layout_string(oldLayout), vk_image_layout_string(newLayout));
                return false;
        }

        // Target layouts (new)
        // The destination access mask controls the dependency for the new image
        // layout.
        switch (newLayout) {
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                // Image will be used as a transfer destination.
                // Make sure any writes to the image have finished.
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                // Image will be used as a transfer source.
                // Make sure any reads from and writes to the image have finished.
                barrier.srcAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                break;

            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                // Image will be used as a color attachment.
                // Make sure any writes to the color buffer have finished.
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                // Image layout will be used as a depth/stencil attachment.
                // Make sure any writes to depth/stencil buffer have finished.
                barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                // Image will be read in a shader (sampler, input attachment).
                // Make sure any writes to the image have finished.
                if (barrier.srcAccessMask == 0)
                {
                    barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                }
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                break;
            default:
                LOG_E("Unsupported layout transition : {0} --> {1}", vk_image_layout_string(oldLayout), vk_image_layout_string(newLayout));
                return false;
        }

        vkCmdPipelineBarrier(
                cmdBuffer,
                srcStage, dstStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
        );
        return true;
    }

}