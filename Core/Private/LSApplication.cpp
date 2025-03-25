#include "LSApplication.h"
#include "Render/LSRenderContext.h"
#include"LSLog.h"
#include"ECS/LSEntity.h"
#include "FPSUtil.h"

namespace lse
{
    LSAppContext LSApplication::sAppContext {};
    void LSApplication::Start(int argc, char **argv) {
        LSLog::Init();

        ParseArgs(argc, argv);
        OnConfiguration(&mAppSettings);

        mWindow = LSWindow::Create(mAppSettings.width, mAppSettings.height,mAppSettings.title);
        mRenderContext = std::make_shared<LSRenderContext>(mWindow.get());

        sAppContext.app = this;
        sAppContext.renderContext  = mRenderContext.get();

        OnInit();
        LoadScene();

        mStartTimePoint = std::chrono::steady_clock::now();
    }

    void LSApplication::Stop() {
        UnLoadScene();
        OnDestroy();
    }

    void LSApplication::MainLoop() {
        mLastTimePoint = std::chrono::steady_clock::now();
        while(!mWindow->ShouldClose()) {
            lse::FPSUtil::calFPS();
            lse::FPSUtil::before();
            mWindow->PollEvents();

            float deltaTime = std::chrono::duration<float>(std::chrono::steady_clock::now()-mLastTimePoint).count();
            mLastTimePoint = std::chrono::steady_clock::now();
            mFrameIndex++;
            if(!bPause) {
                OnUpdate(deltaTime);
            }
            OnRender();

            mWindow->SwapBuffer();
        }
    }

    void LSApplication::ParseArgs(int argc, char *argv[]) {

    }

    bool LSApplication::LoadScene(const std::string &filePath) {
        if(mScene){
            UnLoadScene();
        }
        mScene = std::make_unique<LSScene>();
        OnSceneInit(mScene.get());
        sAppContext.scene = mScene.get();
        return true;
    }

    void LSApplication::UnLoadScene() {
        if(mScene){
            OnSceneDestroy(mScene.get());
            mScene.reset();
            sAppContext.scene = nullptr;
        }
    }

}