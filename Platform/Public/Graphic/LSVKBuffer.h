#ifndef LSVKBUFFER_H
#define LSVKBUFFER_H

#include"LsVKCommon.h"

namespace lse {
    class LSVKDevice;
    class LSVKBuffer {
    public:
        LSVKBuffer(LSVKDevice* device,VkBufferUsageFlags usage,size_t size,void *data = nullptr, bool bHostVisible = false);
        ~LSVKBuffer();

        VkBuffer GetHandle() const {return mHandle;};

        static void CreateBufferInternal(LSVKDevice *device,VkMemoryPropertyFlags memPros,VkBufferUsageFlags usage, size_t size,VkBuffer *outBuffer,VkDeviceMemory *outMemory);
        static void CopyToBuffer(LSVKDevice *device,VkBuffer srcBuffer,VkBuffer dstBuffer,size_t size);

        VkResult WriteData(void *data);
    private:
        void CreateBuffer(VkBufferUsageFlags usage, void *data);

        VkBuffer mHandle = VK_NULL_HANDLE;
        VkDeviceMemory mMemory = VK_NULL_HANDLE;

        LSVKDevice *mDevice;
        size_t mSize;
        bool bHostVisible;
    };


}

#endif //LSVKBUFFER_H
