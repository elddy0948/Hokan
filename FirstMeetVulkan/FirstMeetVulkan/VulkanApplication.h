#pragma once
#ifndef VULKAN_APPLICATION_H
#define VULKAN_APPLICATION_H

#include "VulkanInstance.h"
#include "VulkanDevice.h"

class VulkanApplication {
private:
	VulkanApplication();
public:
	VulkanApplication(const VulkanApplication& rhs) = delete;
	VulkanApplication& operator=(const VulkanApplication& rhs) = delete;
	~VulkanApplication();

public:
	static VulkanApplication* GetApp();

public:
	VulkanInstance* getVulkanInstance() { return &instance; }
	VulkanDevice* getDevice() { return device; }

public:
	void initialize();
	void prepare();
	void update();
	void render();
	void deInitialize();

private:
	VkResult enumeratePhysicalDevices(std::vector<VkPhysicalDevice>& devices);
	VkResult handShakeWithDevice(VkPhysicalDevice* gpu, std::vector<const char*>& layers, std::vector<const char*>& extensions);

private:
	static std::unique_ptr<VulkanApplication> app;
	static std::once_flag onlyOnce;

	VulkanInstance instance;
	VulkanDevice* device = nullptr;
	const char* AppName = "Hokan";
};

#endif /* !VULKAN_APPLICATION_H */
