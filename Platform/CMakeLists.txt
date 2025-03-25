cmake_minimum_required(VERSION 3.29)

project(LnkStain_Engine)

# 定义库 lnkstain_platform
add_library(lnkstain_platform
        Private/LSLog.cpp
        Private/LSWindow.cpp
        Private/Window/LSGLFWwindow.cpp
        Private/FPSUtil.cpp
        Public/LSEngine.h
        Public/LSFileUtil.h

        Private/Graphic/LSGraphicContext.cpp
        Private/Graphic/LSVKGraphicContext.cpp
        Private/Graphic/LSVKDevice.cpp
        Private/Graphic/LSVKQueue.cpp
        Private/Graphic/LSVKCommandBuffer.cpp
        Private/Graphic/LSVKSwapchain.cpp
        Private/Graphic/LSVKRenderPass.cpp
        Private/Graphic/LSVKFramebuffer.cpp
        Private/Graphic/LSVKImageView.cpp
        Private/Graphic/LSVKImage.cpp
        Private/Graphic/LSVKPipeline.cpp
        Private/LSGeometryUtil.cpp
        Private/Graphic/LSVKBuffer.cpp
        Private/Graphic/LSVKDescriptorSet.cpp


        Private/Event/LSEventDispatcher.cpp
)

target_include_directories(lnkstain_platform PUBLIC External)
set(SPDLOG_DIR "${PROJECT_SOURCE_DIR}/External/spdlog")

# 添加 spdlog 的头文件目录
target_include_directories(lnkstain_platform PUBLIC ${SPDLOG_DIR}/include)

set(GLFW_DIR "${PROJECT_SOURCE_DIR}/External/glfw")
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_INSTALL OFF)

add_subdirectory(${GLFW_DIR})
target_include_directories(lnkstain_platform PUBLIC ${GLFW_DIR}/include)
target_link_libraries(lnkstain_platform PRIVATE glfw)

# Vulkan
find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    message("----> Find vulkan success: ${Vulkan_INCLUDE_DIRS}")
endif()
target_include_directories(lnkstain_platform PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(lnkstain_platform PRIVATE ${Vulkan_LIBRARY})