#pragma once
#ifndef VULKAN_LAYER_AND_EXTENSION_H
#define VULKAN_LAYER_AND_EXTENSION_H

#include "Headers.h"

struct LayerProperties {
	VkLayerProperties properties;
	std::vector<VkExtensionProperties> extensions;
};

class VulkanLayerAndExtension {
public:
	VulkanLayerAndExtension();
	VulkanLayerAndExtension(const VulkanLayerAndExtension& rhs) = delete;
	VulkanLayerAndExtension& operator=(const VulkanLayerAndExtension& rhs) = delete;
	~VulkanLayerAndExtension();

public:
	VkResult getInstanceLayerProperties();
	VkResult getExtensionProperties(LayerProperties& layerProperties, VkPhysicalDevice* physicalDevice = NULL);
	VkResult getDeviceExtensionProperties(VkPhysicalDevice* physicalDevice);

	void setExtensionNames(std::vector<const char*> names) { appRequestedExtensionNames = names; }
	void setLayerNames(std::vector<const char*> names) { appRequestedLayerNames = names; }

	std::vector<const char*> getLayerNames() { return appRequestedLayerNames; }
	std::vector<const char*> getExtensionNames() { return appRequestedExtensionNames; }

	VkBool32 areLayersSupported(std::vector<const char*>& layerNames);
	VkResult createDebugReportCallback();
	VkDebugReportCallbackCreateInfoEXT* getDbgReportCreateInfo() { return &dbgReportCreateInfo; }
	void destroyDebugReportCallback();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugFunction(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location, int32_t msgCode, const char* layerPrefix, const char* msg, void* userData);

private:
	std::vector<LayerProperties> layerPropertyList = {};
	std::vector<const char*> appRequestedExtensionNames = {};
	std::vector<const char*> appRequestedLayerNames = {};

	PFN_vkCreateDebugReportCallbackEXT dbgCreateDebugReportCallback = nullptr;
	PFN_vkDestroyDebugReportCallbackEXT dbgDestroyDebugReportCallback = nullptr;

	VkDebugReportCallbackEXT debugReportCallback = nullptr;
	VkDebugReportCallbackCreateInfoEXT dbgReportCreateInfo = {};
};

#endif /* !VULKAN_LAYER_AND_EXTENSION_H */
