#pragma once
#ifndef VULKAN_LAYER_AND_EXTENSION_H
#define VULKAN_LAYER_AND_EXTENSION_H

#include "Headers.h"

class VulkanLayerAndExtension {
public:
	VulkanLayerAndExtension() = default;
	VulkanLayerAndExtension(const VulkanLayerAndExtension& rhs) = delete;
	VulkanLayerAndExtension& operator=(const VulkanLayerAndExtension& rhs) = delete;
	~VulkanLayerAndExtension();

public:
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugFunction(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location, int32_t msgCode, const char* layerPrefix, const char* msg, void* userData);

	VkResult fetchInstanceLayerProperties();
	VkResult fetchInstanceExtensionProperties();
	VkResult fetchDeviceExtensionProperties(VkPhysicalDevice* physicalDevice);

	std::vector<const char*> fetchEnableLayers(std::vector<const char*>& desiredLayers);
	std::vector<const char*> fetchInstanceEnableExtensions(std::vector<const char*>& desiredExtensions);
	std::vector<const char*> fetchDeviceEnableExtensions(std::vector<const char*>& desiredExtensions);

	VkResult createDebugReportCallback();
	VkDebugReportCallbackCreateInfoEXT* getDbgReportCreateInfo() { return &dbgReportCreateInfo; }
	void destroyDebugReportCallback();

private:
	std::vector<VkLayerProperties> layerProperties;
	std::vector<VkExtensionProperties> extensionProperties;
	std::vector<VkExtensionProperties> physicalDeviceExtensionProperties;

	PFN_vkCreateDebugReportCallbackEXT dbgCreateDebugReportCallback = nullptr;
	PFN_vkDestroyDebugReportCallbackEXT dbgDestroyDebugReportCallback = nullptr;

	VkDebugReportCallbackEXT debugReportCallback = nullptr;
	VkDebugReportCallbackCreateInfoEXT dbgReportCreateInfo = {};
};

#endif /* !VULKAN_LAYER_AND_EXTENSION_H */
