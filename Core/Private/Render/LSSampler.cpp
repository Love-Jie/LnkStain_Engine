#include "Render/LSSampler.h"
#include"LSApplication.h"
#include"Render/LSRenderContext.h"

namespace lse {
    LSSampler::LSSampler(VkFilter mFilter, VkSamplerAddressMode mAddressMode) : mFilter(mFilter), mAddressMode(mAddressMode) {
        lse::LSRenderContext *renderContext = LSApplication::GetAppContext()->renderContext;
        lse::LSVKDevice *device = renderContext->GetDevice();

        CALL_VK(device->CreateSimpleSampler(mFilter, mAddressMode, &mHandle));

    }

    LSSampler::~LSSampler() {
        lse::LSRenderContext *renderContext = LSApplication::GetAppContext()->renderContext;
        lse::LSVKDevice *device = renderContext->GetDevice();

        VK_D(Sampler, device->GetHandle(), mHandle);
    }
}
