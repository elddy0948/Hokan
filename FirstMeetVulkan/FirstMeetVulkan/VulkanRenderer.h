#pragma once
#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include "Headers.h"
#include "VulkanSwapChain.h"

#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

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
	inline VulkanDevice* getDevice() { return deviceObject; }
	inline VulkanSwapChain* getSwapChain() { return swapChainObject; }

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

	struct {
		VkFormat format;
		VkImage image;
		VkDeviceMemory deviceMemory;
		VkImageView imageView;
	} Depth;

	VkCommandBuffer commandDepthImage;
	VkCommandPool commandPool;

	int width;
	int height;

private:
	VulkanApplication* application;
	VulkanDevice* deviceObject = nullptr;
	VulkanSwapChain* swapChainObject = nullptr;
};

#endif /* !VULKAN_RENDERER_H */
