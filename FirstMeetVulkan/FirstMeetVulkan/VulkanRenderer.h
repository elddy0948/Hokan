#pragma once
#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include "Headers.h"
#include "VulkanSwapChain.h"
#include "VulkanDrawable.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"

#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

class VulkanRenderer {
public:
	VulkanRenderer(VulkanDevice* device);
	~VulkanRenderer();

public:
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void initialize(const int width, const int height);
	bool render();
	void prepare();

	void setImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask, const VkCommandBuffer& commandBuffer);
	
	VulkanDevice* getDevice() { return deviceObject; }
	VulkanSwapChain* getSwapchain() { return swapChainObject; }
	VkCommandPool* getCommandPool() { return &commandPool; }

	void createCommandPool();
	void buildSwapChainAndDepthImage(); 
	void createDepthImage();

	void createRenderPass(bool includeDepth, bool clear = true);
	void createFrameBuffer(bool includeDepth);

	void createShaders();

	void createPipelineStateManagement();

	void deinitialize();

private:
	void createPresentationWindow();

public:
#ifdef _WIN32
	HINSTANCE connection = nullptr;
	char name[APP_NAME_STR_LEN] = {};
	HWND window = nullptr;
#endif // _WIN32

	struct {
		VkFormat format;
		VkImage image;
		VkDeviceMemory deviceMemory;
		VkImageView imageView;
	} Depth;

	VkCommandBuffer commandDepthImage = nullptr;
	
	unsigned int width = 800;
	unsigned int height = 600;

	VkRenderPass renderPass;
	std::vector<VkFramebuffer> framebuffers;
	std::vector<VulkanDrawable*> drawableList;
	std::vector<VkPipeline*> pipelineList;

private:
	VulkanDevice* deviceObject = nullptr;
	VulkanSwapChain* swapChainObject = nullptr;
	VkCommandPool commandPool = nullptr;
	VulkanShader shaderObject;
	VulkanPipeline pipelineObject;
};

#endif /* !VULKAN_RENDERER_H */
