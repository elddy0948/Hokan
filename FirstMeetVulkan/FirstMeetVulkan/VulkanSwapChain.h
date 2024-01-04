#pragma once
#ifndef VULKAN_SWAP_CHAIN_H
#define VULKAN_SWAP_CHAIN_H

#include "Headers.h"

class VulkanRenderer;
class VulkanApplication;

struct SwapChainBuffer {
	VkImage image;
	VkImageView view;
};

struct SwapChainPrivateVariables {
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	uint32_t presentModeCount;
	std::vector<VkPresentModeKHR> presentModes;
	VkExtent2D swapChainExtent;
	uint32_t desiredNumberOfSwapChainImages;
	VkSurfaceTransformFlagBitsKHR preTransform;
	VkPresentModeKHR swapChainPresentMode;
	std::vector<VkImage> swapChainImages;
	std::vector<VkSurfaceFormatKHR> surfaceFormats;
};

struct SwapChainPublicVariables {
	VkSurfaceKHR surface;
	uint32_t swapChainImageCount;
	VkSwapchainKHR swapChain;
	std::vector<SwapChainBuffer> colorBuffer;
	VkSemaphore presentCompleteSemaphore;
	uint32_t currentColorBuffer;
	VkFormat format;
};

class VulkanSwapChain {
public:
	VulkanSwapChain(VulkanRenderer* renderer);
	~VulkanSwapChain();

public:
	void initializeSwapChain();
	void createSwapChain(const VkCommandBuffer& commandBuffer);
	void destroySwapChain();

private:
	VkResult createSwapChainExtensions();
	void getSupportedFormats();
	VkResult createSurface();
	uint32_t getGraphicsQueueWithPresentationSupport();
	void getSurfaceCapabilitiesAndPresentMode();
	void managePresentMode();
	void createSwapChainColorImages();
	void createColorImageView(const VkCommandBuffer& commandBuffer);

public:
	SwapChainPublicVariables	swapChainPublicVariables	= {};
	PFN_vkQueuePresentKHR		fpQueuePresentKHR			= nullptr;
	PFN_vkAcquireNextImageKHR	fpAcquireNextImageKHR		= nullptr;

private:
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR		fpGetPhysicalDeviceSurfaceSupportKHR		= nullptr;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR	fpGetPhysicalDeviceSurfaceCapabilitiesKHR	= nullptr;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR		fpGetPhysicalDeviceSurfaceFormatsKHR		= nullptr;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR	fpGetPhysicalDeviceSurfacePresentModesKHR	= nullptr;
	PFN_vkDestroySurfaceKHR							fpDestroySurfaceKHR;

	PFN_vkCreateSwapchainKHR	fpCreateSwapchainKHR	= nullptr;
	PFN_vkDestroySwapchainKHR	fpDestroySwapchainKHR	= nullptr;
	PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR	= nullptr;

	SwapChainPrivateVariables	swapchainPrivateVariables = {};
	VulkanRenderer*				rendererObject	= nullptr;
	VulkanApplication*			appObject		= nullptr;
};

#endif /* !VULKAN_SWAP_CHAIN_H */
 