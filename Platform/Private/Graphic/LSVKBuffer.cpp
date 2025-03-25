#include"Graphic/LSVKBuffer.h"
#include"Graphic/LSVKDevice.h"
#include"Graphic/LSVKQueue.h"
#include"Graphic/LSVKCommandBuffer.h"

namespace lse {
    LSVKBuffer::LSVKBuffer(LSVKDevice *device, VkBufferUsageFlags usage, size_t size, void *data,bool bHostVisible)
            : mDevice(device), mSize(size), bHostVisible(bHostVisible) {
        CreateBuffer(usage,data);
    }

    LSVKBuffer::~LSVKBuffer() {
        VK_D(Buffer,mDevice->GetHandle(),mHandle);
        VK_F(mDevice->GetHandle(),mMemory);
    }

    void LSVKBuffer::CreateBuffer(VkBufferUsageFlags usage, void *data) {
        if(bHostVisible){
            CreateBufferInternal(mDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, usage, mSize, &mHandle, &mMemory);
            WriteData(data);
        } else {
            VkBuffer stageBuffer;
            VkDeviceMemory stageMemory;
            CreateBufferInternal(mDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT, mSize, &stageBuffer, &stageMemory);

            void *mapping;
            CALL_VK(vkMapMemory(mDevice->GetHandle(), stageMemory, 0, VK_WHOLE_SIZE, 0, &mapping));
            memcpy(mapping, data, mSize);
            vkUnmapMemory(mDevice->GetHandle(), stageMemory);
            CreateBufferInternal(mDevice, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, mSize, &mHandle, &mMemory);

            // copy
            CopyToBuffer(mDevice, stageBuffer, mHandle, mSize);

            VK_D(Buffer, mDevice->GetHandle(), stageBuffer);
            VK_F(mDevice->GetHandle(), stageMemory);
        }
    }

    void LSVKBuffer::CreateBufferInternal(LSVKDevice *device,VkMemoryPropertyFlags memPros,VkBufferUsageFlags usage, size_t size,VkBuffer *outBuffer,VkDeviceMemory *outMemory)  {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext = nullptr;
        bufferInfo.flags = 0;
        bufferInfo.usage = usage;
        bufferInfo.size = size;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.queueFamilyIndexCount = 0;
        bufferInfo.pQueueFamilyIndices = nullptr;

        CALL_VK(vkCreateBuffer(device->GetHandle(),&bufferInfo,nullptr,outBuffer));

        //allocate memory
        VkMemoryRequirements memReqs;
        vkGetBufferMemoryRequirements(device->GetHandle(),*outBuffer,&memReqs);

        VkMemoryAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.pNext = nullptr;
        allocateInfo.allocationSize = memReqs.size;
        allocateInfo.memoryTypeIndex = static_cast<uint32_t>(device->GetMemoryIndex(memPros,memReqs.memoryTypeBits));

        CALL_VK(vkAllocateMemory(device->GetHandle(),&allocateInfo,nullptr, outMemory));

        CALL_VK(vkBindBufferMemory(device->GetHandle(),*outBuffer,*outMemory,0));

    }

    void LSVKBuffer::CopyToBuffer(LSVKDevice *device,VkBuffer srcBuffer,VkBuffer dstBuffer,size_t size) {
        VkCommandBuffer cmdBuffer = device->CreateAndBeginOneCmdBuffer();
        VkBufferCopy bufferCopy = {};
        bufferCopy.srcOffset = 0;
        bufferCopy.dstOffset = 0;
        bufferCopy.size = size;
        vkCmdCopyBuffer(cmdBuffer,srcBuffer,dstBuffer,1,&bufferCopy);
        LOG_I("src: {0}, dts: {1}",(void*)srcBuffer,(void*)dstBuffer);
        device->SubmitOneCmdBuffer(cmdBuffer);
    }

    VkResult LSVKBuffer::WriteData(void *data) {
        if(data && bHostVisible){
            void *mapping;
            VkResult ret = vkMapMemory(mDevice->GetHandle(), mMemory, 0, VK_WHOLE_SIZE, 0, &mapping);
            memcpy(mapping, data, mSize);
            vkUnmapMemory(mDevice->GetHandle(), mMemory);
            return ret;
        }
        return VK_ERROR_INITIALIZATION_FAILED;
    }

}