#pragma once

#include"LSRenderContext.h"

namespace lse{
#define RENDER_NUM_BUFFER  2
    class LSRenderer{
    public:
        LSRenderer();
        ~LSRenderer();

        bool Begin(int32_t *outImageIndex);
        bool End(int32_t imageIndex,const std::vector<VkCommandBuffer> &cmdBuffers);

    private:
        const uint32_t mNumBuffer = 2;
        uint32_t mCurrentBuffer = 0;
        std::vector<VkSemaphore> mImageAvailableSemaphores;
        std::vector<VkSemaphore> mSubmitedSemaphores;
        std::vector<VkFence> mFrameFences;
    };

}