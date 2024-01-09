#pragma once
#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include "Headers.h"
#include "VulkanLayerAndExtension.h"

class VulkanDevice {
public:
	VulkanDevice(VkPhysicalDevice* physicalDevice);
	~VulkanDevice() {}

public:
	VkResult createLogicalDevice(std::vector<const char*> layers, std::vector<const char*> extensions);

	void fetchQueuesAndProperties();
	void fetchGraphicsQueueHandle();

	void getDeviceQueue() { vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &queue); }
	void destroyDevice() { vkDestroyDevice(device, nullptr); }

	bool memoryTypeFromProperties(uint32_t typeBits, VkFlags requirementsMask, uint32_t* typeIndex);

	VkDevice* getVkDevice() { return &device; }
	VkPhysicalDevice* getPhysicalDevice() { return physicalDevice; }
	VkQueue* getQueue() { return &queue; }

	std::vector<VkQueueFamilyProperties>* getQueueFamilyProperties() { return &queueFamilyProperties; }

	uint32_t getGraphicsQueueFamilyIndex() { return graphicsQueueFamilyIndex; }
	uint32_t getQueueFamilyCount() { return queueFamilyCount; }

	void setGraphcisQueueFamilyIndex(uint32_t index) { graphicsQueueFamilyIndex = index; }

private:
	VkPhysicalDevice* physicalDevice = nullptr;
	VkDevice device = nullptr;
	
	VkPhysicalDeviceProperties deviceProperties = {};
	VkPhysicalDeviceMemoryProperties memoryProperties = {};
	
	VkQueue queue = nullptr;
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;
	uint32_t graphicsQueueFamilyIndex = 0;
	uint32_t queueFamilyCount = 0;

	VulkanLayerAndExtension layerExtension;
};

#endif /* !VULKAN_DEVICE_H */