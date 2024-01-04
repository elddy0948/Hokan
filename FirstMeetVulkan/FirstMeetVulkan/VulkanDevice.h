#pragma once
#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include "Headers.h"
#include "VulkanLayerAndExtension.h"

class VulkanDevice {
public:
	VulkanDevice(VkPhysicalDevice* device) : physicalDevice(device) {}
	~VulkanDevice();

public:
	VkResult createDevice(std::vector<const char*> layers, std::vector<const char*> extensions);
	void getPhysicalDeviceQueuesAndProperties();

	bool getGraphicsQueueHandle();
	void getDeviceQueue();
	void destroyDevice();

	VulkanLayerAndExtension* getLayerExtension() { return &layerExtension; }
	VkPhysicalDeviceProperties* getPhysicalDeviceProperties() { return &physicalDeviceProperties; }
	VkPhysicalDeviceMemoryProperties* getMemoryProperties() { return &memoryProperties; }
	VkDevice* getVkDevice() { return &device; }
	uint32_t getGraphicsQueueFamilyIndex() { return graphicsQueueFamilyIndex; }
	uint32_t getQueueFamilyCount() { return queueFamilyCount; }
	VkPhysicalDevice* getPhysicalDevice() { return physicalDevice; }
	std::vector<VkQueueFamilyProperties>* getQueueFamilyProperties() { return &queueFamilyProperties; }

	void setGraphcisQueueFamilyIndex(uint32_t index) { graphicsQueueFamilyIndex = index; }

private:
	VkPhysicalDevice* physicalDevice;
	VkDevice device = nullptr;
	
	VkPhysicalDeviceProperties physicalDeviceProperties = {};
	VkPhysicalDeviceMemoryProperties memoryProperties = {};
	
	VkQueue queue = nullptr;
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;

	uint32_t graphicsQueueFamilyIndex = 0;
	uint32_t queueFamilyCount = 0;

	VulkanLayerAndExtension layerExtension;
};

#endif /* !VULKAN_DEVICE_H */