cmake_minimum_required(VERSION 3.29)

project(LnkStain_Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} - Os -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} - Os -Wall")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#resource dir configuration
add_definitions(-DLS_DEFINE_RES_ROOT_DIR=\"${CMAKE_SOURCE_DIR}/Resource/\")
set(LS_DEFINE_RES_ROOT_DIR "${CMAKE_SOURCE_DIR}/Resource/")
message("Resource directory: ${LS_DEFINE_RES_ROOT_DIR}")

if(WIN32)
    message("Platform: Windows")
    add_definitions(-DLS_ENGINE_PLATFORM_WIN32)
elseif(UNIX)
    if(APPLE)
        message("Platform: Macos")
        add_definitions(-DLS_ENGINE_PLATFORM_MACOS)
    else()
        message("Platform: Linux")
        add_definitions(-DLS_ENGINE_PLATFORM_LINUX)
    endif()
else()
    message("Platform:Unsupported")
endif()

#vulkan
find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    message("Find vulkan success: ${Vulkan_INCLUDE_DIRS}")
    include_directories(${Vulkan_INCLUDE_DIRS})
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(SPIR_V)

include_directories(Platform/Public)
include_directories(Platform/Public/Window)
include_directories(Platform/Public/Graphic)
include_directories(Core/Public)
include_directories(Core/Public/Render)
include_directories(Core/Public/ECS)
include_directories(Core/Public/ECS/System)
include_directories(Core/Public/ECS/Component)
include_directories(Core/Public/ECS/Component/Material)

add_subdirectory(Platform)
add_subdirectory(Core)
add_subdirectory(Editor)
add_subdirectory(Sample)


