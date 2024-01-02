#pragma once
#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

#include "Headers.h"
#include "VulkanLayerAndExtension.h"

class VulkanInstance {
public:
	VkResult createInstance(std::vector<const char*> layers, std::vector<const char*> extensions, const char* appName);
	void destroyInstance();

private:
	VkInstance instance;
	VulkanLayerAndExtension layerExtension;
};

#endif /* !VULKAN_INSTANCE_H */
