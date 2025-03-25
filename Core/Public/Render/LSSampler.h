#pragma once

#include"Graphic/LSVKCommon.h"

namespace lse{
    class LSSampler{
    public:
        LSSampler(VkFilter mFilter = VK_FILTER_LINEAR,VkSamplerAddressMode mAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);
        ~LSSampler();

        VkSampler GetHandle() const { return mHandle; }

    private:
        VkSampler mHandle = VK_NULL_HANDLE;

        VkFilter mFilter;
        VkSamplerAddressMode mAddressMode;
    };

}