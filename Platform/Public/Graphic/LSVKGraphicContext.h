#pragma once
#include"LSGraphicContext.h"
#include"LSVKCommon.h"
#include"LSWindow.h"

namespace lse {
    struct QueueFamilyInfo {
        int32_t queueFamilyIndex = -1;
        uint32_t queueCount;
    };

    class LSVKGraphicContext :public LSGraphicContext {
    public:
        LSVKGraphicContext(LSWindow* window);
        ~LSVKGraphicContext() override;

        VkInstance GetInstance() const { return mInstance; };
        VkSurfaceKHR  GetSurface() const { return mSurface; };
        VkPhysicalDevice GetPhysicalDevice() const { return mPhysicalDevice; };
        const QueueFamilyInfo& GetGraphicQueueFamilyInfo() const{ return mGraphicQueueFamily; };
        const QueueFamilyInfo& GetPresentQueueFamilyInfo() const { return mPresentQueueFamily; };
        VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties() const { return mPhysicalDeviceMemoryProperties; };
        bool IsSameGraphicPresentQueueFamily() const { return mGraphicQueueFamily.queueFamilyIndex == mPresentQueueFamily.queueFamilyIndex; };

    private:
        static void PrintPhyDeviceInfo(VkPhysicalDeviceProperties& props);   //用于打印物理设备的信息
        static uint32_t GetPhyDeviceScore(VkPhysicalDeviceProperties& props); //用于给物理设备打分

        void CreateInstance();
        void CreateSurface(LSWindow *window);
        void SelectPhysialDevice();

        bool bShouldValidate = true;
        VkInstance mInstance;
        VkSurfaceKHR mSurface;

        VkPhysicalDevice mPhysicalDevice;
        QueueFamilyInfo mGraphicQueueFamily;
        QueueFamilyInfo mPresentQueueFamily;
        VkPhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;  //存储物理设备内存信息，后面的buffer会用到
    };




}