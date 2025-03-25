#pragma once

//该头文件做了差异化处理，包含了所有需要的头文件
#include<iostream>
#include<memory>
#include<algorithm>
#include<functional>
#include<cassert>
#include<stdexcept>
#include<cstdlib>
#include<optional>
#include<Windows.h>

#include<vector>
#include<unordered_set>
#include<map>
#include<set>
#include<queue>
#include<deque>
#include<sstream>
#include<fstream>
#include<filesystem>
#include<stack>
#include<unordered_map>
#include<limits>
#include<cstdint>
#include"LSLog.h"
#include<windows.h>

#ifdef LS_ENGINE_PLATFORM_WIN32
//Windows
#define VK_USE_PLATFORM_WIN32_KHR
#elif LS_ENGINE_PLATFORM_MACOS
//Macos
#define VK_USE_PLATFORM_MACOS_MVK
#elif LS_ENGINE_PLATFORM_LINUX
// Linux
#define VK_USE_PLATFORM_XCB_KHR

#else
    #error Unsuppots this Platform
#endif


#define LS_ENGINE_GRAPHIC_API_VULKAN


#define ARRAY_SIZE(r)    (sizeof(r)/sizeof(r[0]))
#define __FILENAME__     (strrchr(__FILE__, '/') + 1)
#define ENUM_TO_STR(r)   \
    case r: return #r


