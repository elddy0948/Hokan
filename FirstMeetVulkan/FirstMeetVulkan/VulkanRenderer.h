#pragma once
#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include "Headers.h"
#include "VulkanApplication.h"
#include "VulkanDevice.h"

class VulkanRenderer {
public:
	VulkanRenderer(VulkanApplication* app, VulkanDevice* device);
	~VulkanRenderer();

public:
	void initialize();
	bool render();

	void createPresentationWindow(const int& windowWidth = 500, const int& windowHeight = 500);
	void setImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask, const VkCommandBuffer& commandBuffer);
	
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	void destroyPresentationWindow();


	inline VulkanApplication* getApplication() { return application; }
	inline VulkanDevice* getDevice() { return device; }
	//inline VulkanSwapChain* getSwapChain() { return swapChain; }


	void createCommandPool();
	void buildSwapChainAndDepthImage();
	void createDepthImage();

	void destroyCommandBuffer();
	void destroyCommandPool();
	void destroyDepthBuffer();

public:
#ifdef _WIN32
	HINSTANCE connection;
	char name[APP_NAME_STR_LEN];
	HWND window;
#endif // _WIN32

	struct Depth {
		VkFormat format;
		VkImage image;
		VkDeviceMemory deviceMemory;
		VkImageView imageView;
	};

	VkCommandBuffer commandDepthImage;
	VkCommandPool commandPool;

	int width;
	int height;

private:
	VulkanApplication* application;
	VulkanDevice* deviceObject;
	// VulkanSwapChain* swapChain;
};


#endif /* !VULKAN_RENDERER_H */