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

private:
	std::vector<LayerProperties> layerPropertyList = {};
	std::vector<const char*> appRequestedExtensionNames = {};
	std::vector<const char*> appRequestedLayerNames = {};
};

#endif /* !VULKAN_LAYER_AND_EXTENSION_H */
