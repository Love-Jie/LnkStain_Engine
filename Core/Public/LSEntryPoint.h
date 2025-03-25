#pragma once

#include"LSApplication.h"
#include"LSEngine.h"

extern lse::LSApplication *CreateApplicationEntryPoint();

#if LS_ENGINE_PLATFORM_WIN32 || LS_ENGINE_PLATFORM_MACOS || LS_ENGINE_PLATFORM_LINUX

int main(int argc,char *argv[]) {

    std::cout<<"LnkStain Engine starting..."<<std::endl;
    lse::LSApplication *app = CreateApplicationEntryPoint();

    //start
    app->Start(argc,argv);

    //main loop
    app->MainLoop();

    //stop
    app->Stop();

    //destroy
    delete app;

    return EXIT_SUCCESS;
}

#endif