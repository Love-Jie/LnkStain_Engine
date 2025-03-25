#include "LSGraphicContext.h"
#include"LSVKGraphicContext.h"

namespace lse{
    std::unique_ptr<LSGraphicContext>LSGraphicContext::Create(LSWindow* window) {
#ifdef LS_ENGINE_GRAPHIC_API_VULKAN
        return std::make_unique<LSVKGraphicContext>(window);
#else
        return nullptr;
#endif
        return nullptr;
    }
}