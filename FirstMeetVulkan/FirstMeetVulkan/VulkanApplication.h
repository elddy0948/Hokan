#pragma once
#ifndef VULKAN_APPLICATION_H
#define VULKAN_APPLICATION_H

#include "VulkanInstance.h"

class VulkanApplication {
private:
	VulkanApplication();
public:
	VulkanApplication(const VulkanApplication& rhs) = delete;
	VulkanApplication& operator=(const VulkanApplication& rhs) = delete;
	~VulkanApplication();

public:
	static VulkanApplication* GetApp();
	void initialize();

private:
	static std::unique_ptr<VulkanApplication> app;
	static std::once_flag onlyOnce;

	VulkanInstance instance;
	const char* AppName = "Hokan";
};

#endif /* !VULKAN_APPLICATION_H */
