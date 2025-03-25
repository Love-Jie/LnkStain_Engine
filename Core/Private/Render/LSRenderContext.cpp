#include"Render/LSRenderContext.h"

namespace lse {
    LSRenderContext::LSRenderContext(LSWindow *window) {
        mGraphicContext = lse::LSGraphicContext::Create(window);
        auto vkContext = dynamic_cast<lse::LSVKGraphicContext*>(mGraphicContext.get());
        mDevice = std::make_shared<lse::LSVKDevice>(vkContext,1,1);
        mSwapchain = std::make_shared<lse::LSVKSwapchain>(vkContext,mDevice.get());

    }
    LSRenderContext::~LSRenderContext() {

    }



}