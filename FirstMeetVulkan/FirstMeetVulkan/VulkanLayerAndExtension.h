#pragma once
#ifndef VULKAN_LAYER_AND_EXTENSION_H
#define VULKAN_LAYER_AND_EXTENSION_H

#include "Headers.h"

struct LayerProperties {
	VkLayerProperties properties;
	std::vector<VkExtensionProperties> extensions;
};

class VulkanLayerAndExtension {
private:
	VkResult getInstanceLayerProperties();
	VkResult getExtensionProperties(LayerProperties& layerProperties, VkPhysicalDevice* physicalDevice = NULL);
	VkResult getDeviceExtensionProperties(VkPhysicalDevice* physicalDevice);

private:
	std::vector<LayerProperties> layerPropertyList;
};

#endif /* !VULKAN_LAYER_AND_EXTENSION_H */
