#include"LsVKGraphicContext.h"
#include"LSGLFWwindow.h"

namespace lse {
	//验证层
	const DeviceFeature requestedLayers[] = {
		{"VK_LAYER_KHRONOS_validation",true}
	};

	//需要的拓展
	const DeviceFeature requestedExtensions[] = {
		{VK_KHR_SURFACE_EXTENSION_NAME,true},
#ifdef LS_ENGINE_PLATFORM_WIN32
		{VK_KHR_WIN32_SURFACE_EXTENSION_NAME,true},
#elif LS_ENGINE_PLATFORM_MACOS
//Macos
#elif LS_ENGINE_PLATFORM_LINUX
// Linux
#endif
		{VK_EXT_DEBUG_REPORT_EXTENSION_NAME, true}
	};

	LSVKGraphicContext::LSVKGraphicContext(LSWindow* window) {
		CreateInstance();
		CreateSurface(window);
		SelectPhysialDevice();
	}

	LSVKGraphicContext::~LSVKGraphicContext() {
		vkDestroySurfaceKHR(mInstance,mSurface, nullptr);
		vkDestroyInstance(mInstance, nullptr);

	}

	//验证层所需要的函数，需传入到debugReportCallbackInfoEXT中的pfnCallback中去，做日志函数
	static VkBool32 VKAPI_CALL VkDebugReportCallack(VkDebugReportFlagsEXT  flags,
				VkDebugReportObjectTypeEXT                  objectType,
				uint64_t                                    object,
				size_t                                      location,
				int32_t                                     messageCode,
				const char*									pLayerPrefix,
				const char*									pMessage,
				void*										pUserData) {
		if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
			LOG_E("{0}", pMessage);
		}
		if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT || flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
			LOG_W("{0}", pMessage);
		}
		return VK_TRUE;
	}

	//创建实例
	void LSVKGraphicContext::CreateInstance() {
		//1.构建layers
		uint32_t availableLayerCount;
		CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr));
		std::vector<VkLayerProperties> availableLayers(availableLayerCount);
		CALL_VK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()));

		uint32_t enableLayerCount = 0;
		const char* enableLayers[32];
		if (bShouldValidate) {
			if (!checkDeviceFeatures("Instance Layers", false, availableLayerCount, availableLayers.data(),
				ARRAY_SIZE(requestedLayers), requestedLayers, &enableLayerCount, enableLayers)) {
				return;
			}
		}

		//2.构建extensions
		uint32_t availableExtensionCount;
		CALL_VK(vkEnumerateInstanceExtensionProperties("", &availableExtensionCount, nullptr));
		std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
		CALL_VK(vkEnumerateInstanceExtensionProperties("", &availableExtensionCount, availableExtensions.data()));

		//glfw所需要的拓展
		uint32_t glfwRequestedExtensionCount;
		const char** glfwRequestedExtensions = glfwGetRequiredInstanceExtensions(&glfwRequestedExtensionCount);

		std::unordered_set<const char*>allRequestedExtensionSet;
		std::vector<DeviceFeature>allRequestedExtensions;
		//先把原本的Extension加入到allRequestedExtensions中
		for (const auto& item : requestedExtensions) {
			if (allRequestedExtensionSet.find(item.name) == allRequestedExtensionSet.end()) {
				allRequestedExtensionSet.insert(item.name);
				allRequestedExtensions.push_back(item);
			}
		}
		//再把需要的glfw扩展加入到allRequestedExtensions中
		for (uint32_t i = 0; i < glfwRequestedExtensionCount;i++) {
			const char* extensionName = glfwRequestedExtensions[i];
			if (allRequestedExtensionSet.find(extensionName) == allRequestedExtensionSet.end()) {
				allRequestedExtensionSet.insert(extensionName);
				allRequestedExtensions.push_back({ extensionName ,true});
			}
		}
		//测试
		std::cout << "Extensions in unordered_set:" << std::endl;
		for (const auto& extension : allRequestedExtensionSet) {
			std::cout << extension << std::endl;
		}

		size_t requestedExtensionSize = allRequestedExtensions.size();
		uint32_t requestedExtensionCount = static_cast<uint32_t>(requestedExtensionSize);

		uint32_t enableExtensionCount;
		const char* enableExtensions[32];
		if (!checkDeviceFeatures("Instance Extensions", true, availableExtensionCount, availableExtensions.data(), requestedExtensionCount, allRequestedExtensions.data(), &enableExtensionCount, enableExtensions)) {
			return;
		}

		VkApplicationInfo applicationInfo = {};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pNext = nullptr;

		//创建验证层回调日志
		VkDebugReportCallbackCreateInfoEXT debugReportCallbackInfoEXT{};
		debugReportCallbackInfoEXT.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		debugReportCallbackInfoEXT.pNext = nullptr;
		debugReportCallbackInfoEXT.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT;
		debugReportCallbackInfoEXT.pfnCallback = VkDebugReportCallack;

		//3.创建实例
		VkInstanceCreateInfo  instanceInfo = {};
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pNext = bShouldValidate ? &debugReportCallbackInfoEXT : nullptr;
		instanceInfo.flags = 0;
		instanceInfo.pApplicationInfo = &applicationInfo;
		instanceInfo.enabledLayerCount = enableLayerCount;
		instanceInfo.ppEnabledLayerNames = enableLayerCount > 0 ? enableLayers : nullptr;
		instanceInfo.enabledExtensionCount = enableExtensionCount;
		instanceInfo.ppEnabledExtensionNames = enableExtensionCount > 0 ? enableExtensions : nullptr;

		CALL_VK(vkCreateInstance(&instanceInfo, nullptr, &mInstance));

		LOG_T("{0}:instance:{1}", __FUNCTION__,(void*)mInstance);
	}

	//创建窗口表面
	void LSVKGraphicContext::CreateSurface(LSWindow* window) {
		if (!window) {
			LOG_E("window is not exist,");
			return;
		}

		auto *glfwWindow = dynamic_cast<LSGLFWwindow*>(window);
		if (!glfwWindow) {
			LOG_E("this window is not a glfw window.");
			return;
		}

		GLFWwindow *implWindowPointer = static_cast<GLFWwindow*>(glfwWindow->GetImplWindowPointer());
		CALL_VK(glfwCreateWindowSurface(mInstance, implWindowPointer, nullptr, &mSurface));

		LOG_T("{0}:instance:{1}", __FUNCTION__, (void*)mSurface);

	}

	//选择物理设备
	void LSVKGraphicContext::SelectPhysialDevice() {
		uint32_t physicalDeviceCount;
		CALL_VK(vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, nullptr));
		std::vector<VkPhysicalDevice>physicalDevices(physicalDeviceCount);
		CALL_VK(vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, physicalDevices.data()));

		//为物理设备打分并选择分最高的物理设备
		uint32_t maxScore = 0;
		int32_t maxScorePhyDeviceIndex = -1;
		LOG_D("--------------------");
		LOG_D("Physical Device: ");

		for (uint32_t i = 0; i < physicalDeviceCount; i++) {
			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties(physicalDevices[i], &props);
			PrintPhyDeviceInfo(props);

			uint32_t score = GetPhyDeviceScore(props);

			//支持的格式
			uint32_t formatCount;
			CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[i], mSurface, &formatCount, nullptr));
			std::vector<VkSurfaceFormatKHR>formats(formatCount);
			CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[i], mSurface, &formatCount, formats.data()));
			for (uint32_t j = 0; j < formatCount; j++) {
				if (formats[j].format == VK_FORMAT_B8G8R8A8_UNORM && formats[j].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
					score += 10;
					break;
				}
			}


			//查询队列族
			uint32_t queueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, nullptr);
			std::vector<VkQueueFamilyProperties>queueFamilys(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, queueFamilys.data());

			LOG_D("score    --->    :{0}", score);
			LOG_D("queue family     :{0}", queueFamilyCount);

			if (score < maxScore) {
				continue;
			}

			for (uint32_t j = 0; j < queueFamilyCount; j++) {
				if (queueFamilys[j].queueCount == 0) {
					continue;
				}

				//graphic family
				if (queueFamilys[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					mGraphicQueueFamily.queueFamilyIndex = j;
					mGraphicQueueFamily.queueCount = queueFamilys[j].queueCount;
				}

				//present family
				VkBool32 bSupportSurface; //present队列是否适合当前的surface
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[i], j, mSurface, &bSupportSurface);
				if (bSupportSurface) {
					mPresentQueueFamily.queueFamilyIndex = j;
					mPresentQueueFamily.queueCount = queueFamilys[j].queueCount;
				}

				if (mGraphicQueueFamily.queueFamilyIndex >= 0 && mPresentQueueFamily.queueFamilyIndex >= 0
					&& mGraphicQueueFamily.queueFamilyIndex != mPresentQueueFamily.queueFamilyIndex) {
					break;
				}

			}
			if (mGraphicQueueFamily.queueFamilyIndex >= 0 && mPresentQueueFamily.queueFamilyIndex >= 0) {
				maxScorePhyDeviceIndex = i;
				maxScore = score;
			}
		}

		LOG_D("--------------------");

		if (maxScorePhyDeviceIndex < 0) {
			LOG_W("Maybe can not find a suitable device, will 0.");
			maxScorePhyDeviceIndex = 0;
		}

		mPhysicalDevice = physicalDevices[maxScorePhyDeviceIndex];
		vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mPhysicalDeviceMemoryProperties);
		LOG_T("{0}: physical device: {1}, score: {2}, graphic queue: {3} : {4}, present queue: {5} : {6}", __FUNCTION__, maxScorePhyDeviceIndex, maxScore,
			mGraphicQueueFamily.queueFamilyIndex, mGraphicQueueFamily.queueCount,
			mPresentQueueFamily.queueFamilyIndex, mPresentQueueFamily.queueCount
		);
	}

	//用于打印物理设备的信息
	void LSVKGraphicContext::PrintPhyDeviceInfo(VkPhysicalDeviceProperties &props) {

		const char* deviceType = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? "integrated gpu" :
			props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "discrete gpu" :
			props.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU ? "virtual gpu" :
			props.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU ? "cpu" :
			"others";

		uint32_t driverVersionMajor = VK_VERSION_MAJOR(props.driverVersion);
		uint32_t driverVersionMinor = VK_VERSION_MINOR(props.driverVersion);
		uint32_t driverVersionPatch = VK_VERSION_PATCH(props.driverVersion);

		uint32_t apiVersionMajor = VK_VERSION_MAJOR(props.apiVersion);
		uint32_t apiVersionMinor = VK_VERSION_MINOR(props.apiVersion);
		uint32_t apiVersionPatch = VK_VERSION_PATCH(props.apiVersion);

		LOG_D("--------------------");
		LOG_D("deviceName      : {0}", props.deviceName);
		LOG_D("deviceType      : {0}", deviceType);
		LOG_D("vendorID        : {0}", props.vendorID);
		LOG_D("deviceID        : {0}", props.deviceID);
		LOG_D("driverVersion   : {0},{1},{2}", driverVersionMajor,driverVersionMinor,driverVersionPatch);
		LOG_D("apiVersion      : {0},{1},{2}", apiVersionMajor,apiVersionMinor,apiVersionPatch);

	}

	//给物理设备打分
	uint32_t LSVKGraphicContext::GetPhyDeviceScore(VkPhysicalDeviceProperties& props) {
		VkPhysicalDeviceType deviceType = props.deviceType;
		uint32_t score = 0;
		switch (deviceType) {
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			score += 50;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			score += 40;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			score += 30;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			score += 20;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			score += 10;
			break;
		default:
			score += 0;
			break;
		}
		return score;
	}
}