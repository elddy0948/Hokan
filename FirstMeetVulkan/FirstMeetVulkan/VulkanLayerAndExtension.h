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
	VkResult getInstanceLayerProperties();
	VkResult getExtensionProperties(LayerProperties& layerProperties, VkPhysicalDevice* physicalDevice = NULL);
	VkResult getDeviceExtensionProperties(VkPhysicalDevice* physicalDevice);

	void setInstanceLayerNames(std::vector<const char*> names) { instanceLayerNames = names; }
	void setInstanceExtensionNames(std::vector<const char*> names) { instanceExtensionNames = names; }

private:
	std::vector<LayerProperties> layerPropertyList;
	std::vector<const char*> instanceLayerNames;
	std::vector<const char*> instanceExtensionNames;
};

#endif /* !VULKAN_LAYER_AND_EXTENSION_H */
