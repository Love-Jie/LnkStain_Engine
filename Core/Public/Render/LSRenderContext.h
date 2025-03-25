#pragma once

#include"Graphic/LSVKGraphicContext.h"
#include"Graphic/LSVKSwapchain.h"
#include"Graphic/LSVKDevice.h"

namespace lse{
    class LSWindow;

    class LSRenderContext{
    public:
        LSRenderContext(LSWindow *window);
        ~LSRenderContext();

        LSGraphicContext *GetGraphicsContext() const {return mGraphicContext.get();};
        LSVKDevice *GetDevice() const {return mDevice.get();};
        LSVKSwapchain *GetSwapchain() const {return mSwapchain.get();};

    private:
        std::shared_ptr<LSGraphicContext> mGraphicContext;
        std::shared_ptr<LSVKDevice> mDevice;
        std::shared_ptr<LSVKSwapchain> mSwapchain;
    };

}