#pragma once
#include"LsVKDevice.h"

namespace lse {
    class LSVKDevice;
    class LSVKCommandPool {
    public:
        LSVKCommandPool(LSVKDevice *device,uint32_t queueFamilyIndex);
        ~LSVKCommandPool();

        static void BeginCommandBuffer(VkCommandBuffer cmdBuffer);
        static void EndCommandBuffer(VkCommandBuffer cmdBuffer);

        std::vector<VkCommandBuffer> AllocateCommandBuffers(uint32_t count) const;
        VkCommandBuffer AllocateOneCommandBuffer() const;

        VkCommandPool GetHandle() const { return mHandle;};

    private:
        VkCommandPool mHandle;
        LSVKDevice *mDevice;

    };

}

