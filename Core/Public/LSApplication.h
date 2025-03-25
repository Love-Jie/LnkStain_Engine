#pragma once

#include"LSWindow.h"
#include"LSApplicationContext.h"

namespace lse
{
    struct AppSettings {
        uint32_t width = 800;
        uint32_t height = 600;
        const char* title = "LnkStain Engine";
    };

    class LSApplication
    {
    public:
        static LSAppContext *GetAppContext() {return &sAppContext;};

        void Start(int argc, char *argv[]);
        void Stop();
        void MainLoop();

        void Pause() {bPause = true;};
        bool IsPause() const {return bPause;}
        void Resume() {if(bPause) bPause = false;}

        float GetStartTimeSecond() const {return std::chrono::duration<float>(std::chrono::steady_clock::now() - mStartTimePoint).count();}
        uint64_t GetFrameIndex() const { return mFrameIndex;}
    protected:
        virtual void OnConfiguration(AppSettings *appSettings){}
        virtual void OnInit(){}
        virtual void OnUpdate(float deltaTime){}
        virtual void OnRender(){}
        virtual void OnDestroy(){}

        virtual void OnSceneInit(LSScene *scene){};
        virtual void OnSceneDestroy(LSScene *scene){};

        std::chrono::steady_clock::time_point mStartTimePoint;
        std::chrono::steady_clock::time_point mLastTimePoint;
        std::shared_ptr<LSRenderContext> mRenderContext;
        std::unique_ptr<LSWindow> mWindow;
        std::unique_ptr<LSScene> mScene;

    private:
        void ParseArgs(int argc, char *argv[]);
        bool LoadScene(const std::string &filePath = "");
        void UnLoadScene();


        AppSettings mAppSettings;

        uint64_t mFrameIndex = 0;
        bool bPause = false;

        static LSAppContext sAppContext;
    };

}