#include"LsVKCommandBuffer.h"
#include"LsVKDevice.h"

namespace lse {
    LSVKCommandPool::LSVKCommandPool(LSVKDevice *device,uint32_t queueFamilyIndex) : mDevice(device) {
        VkCommandPoolCreateInfo commandPoolInfo = {};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.pNext = nullptr;
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolInfo.queueFamilyIndex = queueFamilyIndex;

        CALL_VK(vkCreateCommandPool(mDevice->GetHandle(),&commandPoolInfo,nullptr,&mHandle));

        LOG_T("Create command pool: {0}", (void*)mHandle);
    }

    LSVKCommandPool::~LSVKCommandPool() {
        vkDestroyCommandPool(mDevice->GetHandle(),mHandle,nullptr);
    }

    std::vector<VkCommandBuffer> LSVKCommandPool::AllocateCommandBuffers(uint32_t count) const {
        std::vector<VkCommandBuffer> commandBuffers(count);
        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.pNext = nullptr;
        allocateInfo.commandPool = mHandle;
        allocateInfo.commandBufferCount = count;

        CALL_VK(vkAllocateCommandBuffers(mDevice->GetHandle(),&allocateInfo,commandBuffers.data()));

        return commandBuffers;
    }

   VkCommandBuffer LSVKCommandPool::AllocateOneCommandBuffer() const {
        std::vector<VkCommandBuffer> cmdBuffers = AllocateCommandBuffers(1);
        return cmdBuffers[0];
   }


    void LSVKCommandPool::BeginCommandBuffer(VkCommandBuffer cmdBuffer) {
        CALL_VK(vkResetCommandBuffer(cmdBuffer,0));
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pNext = nullptr;

        CALL_VK(vkBeginCommandBuffer(cmdBuffer,&beginInfo));
    }

    void LSVKCommandPool::EndCommandBuffer(VkCommandBuffer cmdBuffer) {
        CALL_VK(vkEndCommandBuffer(cmdBuffer));
    }
}
