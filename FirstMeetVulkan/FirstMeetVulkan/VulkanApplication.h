#pragma once
#ifndef VULKAN_APPLICATION_H
#define VULKAN_APPLICATION_H

#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanRenderer.h"

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
	VulkanDevice* getVulkanDevice() { return device; }
	VulkanRenderer* getRenderer() { return rendererObject; }

public:
	/* Life Cycle */
	void initialize();
	void prepare();
	void update();
	bool render();
	void deInitialize();

private:
	VkResult enumeratePhysicalDevices(std::vector<VkPhysicalDevice>& devices);
	VkResult handShakeWithDevice(VkPhysicalDevice* gpu, std::vector<const char*>& layers, std::vector<const char*>& extensions);

private:
	static std::unique_ptr<VulkanApplication> app;
	static std::once_flag onlyOnce;

private:
	VulkanInstance instance;
	VulkanDevice* device = nullptr;
	VulkanRenderer* rendererObject = nullptr;

	const char* AppName = "Hokan";
	bool debugFlag = false;
};

#endif /* !VULKAN_APPLICATION_H */
