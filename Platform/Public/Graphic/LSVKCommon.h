#pragma once
#include<windows.h>
#include<vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include"LSLog.h"

//设备特征
struct DeviceFeature {
	const char* name;  //拓展名
	bool required;   //是否必须
};

static bool checkDeviceFeatures(const char* Lable, bool bExtension, uint32_t availableCount, void* available,
							uint32_t requestedCount, const DeviceFeature* requestFeatures, uint32_t* outEnableCount,
							const char* outEnableFeatures[]) {
	bool bFoundAllFeatures = true;

	*outEnableCount = 0;

	LOG_D("---------------------------");

	LOG_D("{0}", Lable);
	for (uint32_t i = 0; i < requestedCount; i++) {
		bool bFound = false;
		const char* result = requestFeatures[i].required ? "(required, not found)" : ("not found");

		for (uint32_t j = 0; i < availableCount; j++) {
			const char* availableName = bExtension ? ((VkExtensionProperties*)(available))[j].extensionName : ((VkLayerProperties*)(available))[j].layerName;
			if (strcmp(availableName, requestFeatures[i].name) == 0) {
				outEnableFeatures[(*outEnableCount)++] = availableName;
				bFound = true;
				break;
			}
		}

		if (bFound) {
			result = requestFeatures[i].required ? "(required,enable)" : "(enable)";

		}
		bFoundAllFeatures &= bFound || requestFeatures[i].required;

		LOG_D("{0},{1}", requestFeatures[i].name, result);
	}
	LOG_D("---------------------------");
	return bFoundAllFeatures;
}

#define CALL_VK(func) check_vk_result(func,__FILE_NAME__,__LINE__,#func)
#define VK_D(type,device,obj) if(obj!=VK_SUCCESS) vkDestroy##type(device,obj,nullptr)
#define VK_F(device,mem) if(mem!=VK_SUCCESS) vkFreeMemory(device,mem,nullptr)



static const char*
vk_result_string(VkResult code) {
    switch (code) {
        ENUM_TO_STR(VK_SUCCESS);
        ENUM_TO_STR(VK_NOT_READY);
        ENUM_TO_STR(VK_TIMEOUT);
        ENUM_TO_STR(VK_EVENT_SET);
        ENUM_TO_STR(VK_EVENT_RESET);
        ENUM_TO_STR(VK_INCOMPLETE);
        ENUM_TO_STR(VK_ERROR_OUT_OF_HOST_MEMORY);
        ENUM_TO_STR(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        ENUM_TO_STR(VK_ERROR_INITIALIZATION_FAILED);
        ENUM_TO_STR(VK_ERROR_DEVICE_LOST);
        ENUM_TO_STR(VK_ERROR_MEMORY_MAP_FAILED);
        ENUM_TO_STR(VK_ERROR_LAYER_NOT_PRESENT);
        ENUM_TO_STR(VK_ERROR_EXTENSION_NOT_PRESENT);
        ENUM_TO_STR(VK_ERROR_FEATURE_NOT_PRESENT);
        ENUM_TO_STR(VK_ERROR_INCOMPATIBLE_DRIVER);
        ENUM_TO_STR(VK_ERROR_TOO_MANY_OBJECTS);
        ENUM_TO_STR(VK_ERROR_FORMAT_NOT_SUPPORTED);
        ENUM_TO_STR(VK_ERROR_FRAGMENTED_POOL);
#ifdef VK_VERSION_1_1
        ENUM_TO_STR(VK_ERROR_OUT_OF_POOL_MEMORY);
        ENUM_TO_STR(VK_ERROR_INVALID_EXTERNAL_HANDLE);
#endif
#ifdef VK_VERSION_1_2
        ENUM_TO_STR(VK_ERROR_UNKNOWN); // Only defined in 1.2 and above headers.
        ENUM_TO_STR(VK_ERROR_FRAGMENTATION);
        ENUM_TO_STR(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
#else
    case -13 /* VK_ERROR_UNKNOWN */: return "VK_ERROR_UNKNOWN"; // Has no guard.
#endif
#ifdef VK_VERSION_1_3
        ENUM_TO_STR(VK_PIPELINE_COMPILE_REQUIRED);
#endif
#ifdef VK_KHR_surface
        ENUM_TO_STR(VK_ERROR_SURFACE_LOST_KHR);
        ENUM_TO_STR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
#endif
#ifdef VK_KHR_swapchain
        ENUM_TO_STR(VK_SUBOPTIMAL_KHR);
        ENUM_TO_STR(VK_ERROR_OUT_OF_DATE_KHR);
#endif
#ifdef VK_KHR_display_swapchain
        ENUM_TO_STR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
#endif
#ifdef VK_EXT_debug_report
        ENUM_TO_STR(VK_ERROR_VALIDATION_FAILED_EXT);
#endif
#ifdef VK_NV_glsl_shader
        ENUM_TO_STR(VK_ERROR_INVALID_SHADER_NV);
#endif
#if defined(VK_ENABLE_BETA_EXTENSIONS) && defined(VK_KHR_video_queue)
        ENUM_TO_STR(VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR);
        ENUM_TO_STR(VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR);
        ENUM_TO_STR(VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR);
        ENUM_TO_STR(VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);
        ENUM_TO_STR(VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR);
        ENUM_TO_STR(VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR);
#endif
#ifdef VK_EXT_image_drm_format_modifier
        ENUM_TO_STR(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
#endif
#ifdef VK_KHR_global_priority
        ENUM_TO_STR(VK_ERROR_NOT_PERMITTED_KHR);
#endif
#ifdef VK_EXT_full_screen_exclusive
        ENUM_TO_STR(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
#endif
#ifdef VK_KHR_deferred_host_operations
        ENUM_TO_STR(VK_THREAD_IDLE_KHR);
#endif
#ifdef VK_KHR_deferred_host_operations
        ENUM_TO_STR(VK_THREAD_DONE_KHR);
#endif
#ifdef VK_KHR_deferred_host_operations
        ENUM_TO_STR(VK_OPERATION_DEFERRED_KHR);
#endif
#ifdef VK_KHR_deferred_host_operations
        ENUM_TO_STR(VK_OPERATION_NOT_DEFERRED_KHR);
#endif
#ifdef VK_EXT_image_compression_control
        ENUM_TO_STR(VK_ERROR_COMPRESSION_EXHAUSTED_EXT);
#endif
#if defined(VK_KHR_maintenance1) && !defined(VK_VERSION_1_1)
        ENUM_TO_STR(VK_ERROR_OUT_OF_POOL_MEMORY_KHR);
#endif
#if defined(VK_KHR_external_memory) && !defined(VK_VERSION_1_1)
        ENUM_TO_STR(VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR);
#endif
#if defined(VK_EXT_descriptor_indexing) && !defined(VK_VERSION_1_2)
        ENUM_TO_STR(VK_ERROR_FRAGMENTATION_EXT);
#endif
#if defined(VK_EXT_global_priority) && !defined(VK_KHR_global_priority)
        ENUM_TO_STR(VK_ERROR_NOT_PERMITTED_EXT);
#endif
#if defined(VK_EXT_buffer_device_address) && !defined(VK_VERSION_1_2)
        ENUM_TO_STR(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT);
        // VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR = VK_ERROR_INVALID_DEVICE_ADDRESS_EXT
#endif
#if defined(VK_EXT_pipeline_creation_cache_control) && !defined(VK_VERSION_1_3)
        ENUM_TO_STR(VK_PIPELINE_COMPILE_REQUIRED_EXT);
        // VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT = VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT
#endif
    default: return "UNKNOWN RESULT";
    }
}

static void check_vk_result(VkResult result,const char*fileName,uint32_t line,const char* func) {
        if(result != VK_SUCCESS) {
                LOG_E("{0}:{1}:{2} --->{3}",fileName,line,func,vk_result_string(result));
        }

}

static const char* vk_image_layout_string(VkImageLayout layout){
    switch (layout) {
        ENUM_TO_STR(VK_IMAGE_LAYOUT_UNDEFINED);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_GENERAL);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_PREINITIALIZED);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
#ifdef VK_ENABLE_BETA_EXTENSIONS
            ENUM_TO_STR(VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR);
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
            ENUM_TO_STR(VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR);
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
            ENUM_TO_STR(VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR);
#endif
        ENUM_TO_STR(VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT);
        ENUM_TO_STR(VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR);
#ifdef VK_ENABLE_BETA_EXTENSIONS
            ENUM_TO_STR(VK_IMAGE_LAYOUT_VIDEO_ENCODE_DST_KHR);
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
            ENUM_TO_STR(VK_IMAGE_LAYOUT_VIDEO_ENCODE_SRC_KHR);
#endif
#ifdef VK_ENABLE_BETA_EXTENSIONS
            ENUM_TO_STR(VK_IMAGE_LAYOUT_VIDEO_ENCODE_DPB_KHR);
#endif
    }
    return "";
}



//用于将VkFormat类型值转化为字符串
static const char* vk_format_string(VkFormat format) {
    switch (format) {
        ENUM_TO_STR(VK_FORMAT_UNDEFINED);
        ENUM_TO_STR(VK_FORMAT_R4G4_UNORM_PACK8);
        ENUM_TO_STR(VK_FORMAT_R4G4B4A4_UNORM_PACK16);
        ENUM_TO_STR(VK_FORMAT_B4G4R4A4_UNORM_PACK16);
        ENUM_TO_STR(VK_FORMAT_R5G6B5_UNORM_PACK16);
        ENUM_TO_STR(VK_FORMAT_B5G6R5_UNORM_PACK16);
        ENUM_TO_STR(VK_FORMAT_R5G5B5A1_UNORM_PACK16);
        ENUM_TO_STR(VK_FORMAT_B5G5R5A1_UNORM_PACK16);
        ENUM_TO_STR(VK_FORMAT_A1R5G5B5_UNORM_PACK16);
        ENUM_TO_STR(VK_FORMAT_R8_UNORM);
        ENUM_TO_STR(VK_FORMAT_R8_SNORM);
        ENUM_TO_STR(VK_FORMAT_R8_USCALED);
        ENUM_TO_STR(VK_FORMAT_R8_SSCALED);
        ENUM_TO_STR(VK_FORMAT_R8_UINT);
        ENUM_TO_STR(VK_FORMAT_R8_SINT);
        ENUM_TO_STR(VK_FORMAT_R8_SRGB);
        ENUM_TO_STR(VK_FORMAT_R8G8_UNORM);
        ENUM_TO_STR(VK_FORMAT_R8G8_SNORM);
        ENUM_TO_STR(VK_FORMAT_R8G8_USCALED);
        ENUM_TO_STR(VK_FORMAT_R8G8_SSCALED);
        ENUM_TO_STR(VK_FORMAT_R8G8_UINT);
        ENUM_TO_STR(VK_FORMAT_R8G8_SINT);
        ENUM_TO_STR(VK_FORMAT_R8G8_SRGB);
        ENUM_TO_STR(VK_FORMAT_R8G8B8_UNORM);
        ENUM_TO_STR(VK_FORMAT_R8G8B8_SNORM);
        ENUM_TO_STR(VK_FORMAT_R8G8B8_USCALED);
        ENUM_TO_STR(VK_FORMAT_R8G8B8_SSCALED);
        ENUM_TO_STR(VK_FORMAT_R8G8B8_UINT);
        ENUM_TO_STR(VK_FORMAT_R8G8B8_SINT);
        ENUM_TO_STR(VK_FORMAT_R8G8B8_SRGB);
        ENUM_TO_STR(VK_FORMAT_B8G8R8_UNORM);
        ENUM_TO_STR(VK_FORMAT_B8G8R8_SNORM);
        ENUM_TO_STR(VK_FORMAT_B8G8R8_USCALED);
        ENUM_TO_STR(VK_FORMAT_B8G8R8_SSCALED);
        ENUM_TO_STR(VK_FORMAT_B8G8R8_UINT);
        ENUM_TO_STR(VK_FORMAT_B8G8R8_SINT);
        ENUM_TO_STR(VK_FORMAT_B8G8R8_SRGB);
        ENUM_TO_STR(VK_FORMAT_R8G8B8A8_UNORM);
        ENUM_TO_STR(VK_FORMAT_R8G8B8A8_SNORM);
        ENUM_TO_STR(VK_FORMAT_R8G8B8A8_USCALED);
        ENUM_TO_STR(VK_FORMAT_R8G8B8A8_SSCALED);
        ENUM_TO_STR(VK_FORMAT_R8G8B8A8_UINT);
        ENUM_TO_STR(VK_FORMAT_R8G8B8A8_SINT);
        ENUM_TO_STR(VK_FORMAT_R8G8B8A8_SRGB);
        ENUM_TO_STR(VK_FORMAT_B8G8R8A8_UNORM);
        ENUM_TO_STR(VK_FORMAT_B8G8R8A8_SNORM);
        ENUM_TO_STR(VK_FORMAT_B8G8R8A8_USCALED);
        ENUM_TO_STR(VK_FORMAT_B8G8R8A8_SSCALED);
        ENUM_TO_STR(VK_FORMAT_B8G8R8A8_UINT);
        ENUM_TO_STR(VK_FORMAT_B8G8R8A8_SINT);
        ENUM_TO_STR(VK_FORMAT_B8G8R8A8_SRGB);
        ENUM_TO_STR(VK_FORMAT_A8B8G8R8_UNORM_PACK32);
        ENUM_TO_STR(VK_FORMAT_A8B8G8R8_SNORM_PACK32);
        ENUM_TO_STR(VK_FORMAT_A8B8G8R8_USCALED_PACK32);
        ENUM_TO_STR(VK_FORMAT_A8B8G8R8_SSCALED_PACK32);
        ENUM_TO_STR(VK_FORMAT_A8B8G8R8_UINT_PACK32);
        ENUM_TO_STR(VK_FORMAT_A8B8G8R8_SINT_PACK32);
        ENUM_TO_STR(VK_FORMAT_A8B8G8R8_SRGB_PACK32);
        ENUM_TO_STR(VK_FORMAT_A2R10G10B10_UNORM_PACK32);
        ENUM_TO_STR(VK_FORMAT_A2R10G10B10_SNORM_PACK32);
        ENUM_TO_STR(VK_FORMAT_A2R10G10B10_USCALED_PACK32);
        ENUM_TO_STR(VK_FORMAT_A2R10G10B10_SSCALED_PACK32);
        ENUM_TO_STR(VK_FORMAT_A2R10G10B10_UINT_PACK32);
        ENUM_TO_STR(VK_FORMAT_A2R10G10B10_SINT_PACK32);
        ENUM_TO_STR(VK_FORMAT_A2B10G10R10_UNORM_PACK32);
        ENUM_TO_STR(VK_FORMAT_A2B10G10R10_SNORM_PACK32);
        ENUM_TO_STR(VK_FORMAT_A2B10G10R10_USCALED_PACK32);
        ENUM_TO_STR(VK_FORMAT_A2B10G10R10_SSCALED_PACK32);
        ENUM_TO_STR(VK_FORMAT_A2B10G10R10_UINT_PACK32);
        ENUM_TO_STR(VK_FORMAT_A2B10G10R10_SINT_PACK32);
    default:
        return "";
    }
}

static const char* vk_present_mode_string(VkPresentModeKHR presentMode) {
    switch (presentMode) {
        ENUM_TO_STR(VK_PRESENT_MODE_IMMEDIATE_KHR);
        ENUM_TO_STR(VK_PRESENT_MODE_MAILBOX_KHR);
        ENUM_TO_STR(VK_PRESENT_MODE_FIFO_KHR);
        ENUM_TO_STR(VK_PRESENT_MODE_FIFO_RELAXED_KHR);
        ENUM_TO_STR(VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR);
        ENUM_TO_STR(VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR);
        // Add other VkPresentModeKHR enum values as needed
    default:
        return "";
    }
}
//用于判断格式是否是深度格式
static bool IsDepthOnlyFormat(VkFormat format) {
        return format == VK_FORMAT_D16_UNORM || format == VK_FORMAT_D32_SFLOAT;
}
//判断是否为深度模版
static bool IsDepthStencilFormat(VkFormat format) {
        return  IsDepthOnlyFormat(format)
        ||format == VK_FORMAT_D16_UNORM_S8_UINT
        ||format == VK_FORMAT_D24_UNORM_S8_UINT
        ||format == VK_FORMAT_D32_SFLOAT_S8_UINT;
}
