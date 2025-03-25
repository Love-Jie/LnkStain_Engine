#pragma once

namespace lse {
    class LSApplication;
    class LSScene;
    class LSRenderContext;

    struct LSAppContext {
        LSApplication *app;
        LSScene *scene;
        LSRenderContext *renderContext;
    };

}