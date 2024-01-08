#pragma once
#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

#include "VulkanLayerAndExtension.h"

class VulkanInstance {
public:
	VulkanInstance() = default;
	VulkanInstance(const VulkanInstance& rhs) = delete;
	VulkanInstance& operator=(const VulkanInstance& rhs) = delete;
	~VulkanInstance() {}

public:
	void initialize();
	VkResult createInstance(std::vector<const char*>& layers, std::vector<const char*>& extensions, const char* appName);
	void setupDebugLayer();
	void destroyInstance();


	VkInstance* getVkInstance() { return &instance; }
	VulkanLayerAndExtension* getLayerExtension() { return &layerExtension; }

private:
	VkInstance instance = nullptr;
	VulkanLayerAndExtension layerExtension;
};

#endif /* !VULKAN_INSTANCE_H */