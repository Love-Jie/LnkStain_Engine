#pragma once

namespace lse{
    class FPSUtil
    {
    public:
        static int FPSCount;
        static long long startTime;
        static float currFPS;
        static long long beforeTime;
        static void init();
        static void calFPS();
        static void before();
        static void after(int dstFPS);
    };

}

