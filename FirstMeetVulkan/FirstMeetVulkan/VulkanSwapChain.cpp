#include "VulkanSwapChain.h"
#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "VulkanRenderer.h"
#include "VulkanApplication.h"

#define INSTANCE_FUNC_PTR(instance, entrypoint){											\
    fp##entrypoint = (PFN_vk##entrypoint) vkGetInstanceProcAddr(instance, "vk"#entrypoint); \
    if (fp##entrypoint == NULL) {															\
        std::cout << "Unable to locate the vkGetDeviceProcAddr: vk"#entrypoint;				\
        exit(-1);																			\
    }																						\
}

#define DEVICE_FUNC_PTR(dev, entrypoint){													\
    fp##entrypoint = (PFN_vk##entrypoint) vkGetDeviceProcAddr(dev, "vk"#entrypoint);		\
    if (fp##entrypoint == NULL) {															\
        std::cout << "Unable to locate the vkGetDeviceProcAddr: vk"#entrypoint;				\
        exit(-1);																			\
    }																						\
}

VulkanSwapChain::VulkanSwapChain(VulkanRenderer* renderer) {
	rendererObject = renderer;
	appObject = VulkanApplication::GetApp();
}

VulkanSwapChain::~VulkanSwapChain() {
	swapchainPrivateVariables.swapChainImages.clear();
	swapchainPrivateVariables.surfaceFormats.clear();
	swapchainPrivateVariables.presentModes.clear();
}

void VulkanSwapChain::initializeSwapChain() {
	createSwapChainExtensions();
	createSurface();

	uint32_t index = getGraphicsQueueWithPresentationSupport();

	if (index == UINT32_MAX) {
		std::cout << "Could not find a graphics and a present queue\nCould not find a graphics and a present queue.\n";
		exit(-1);
	}

	rendererObject->getDevice()->setGraphcisQueueFamilyIndex(index);
	getSupportedFormats();
}

void VulkanSwapChain::createSwapChain(const VkCommandBuffer& commandBuffer) {
	getSurfaceCapabilitiesAndPresentMode();
	managePresentMode();
}

void VulkanSwapChain::destroySwapChain() {
	VulkanDevice* deviceObject = appObject->getVulkanDevice();
	for (uint32_t i = 0; i < swapChainPublicVariables.swapChainImageCount; ++i) {
		vkDestroyImageView(*deviceObject->getVkDevice(), swapChainPublicVariables.colorBuffer[i].view, nullptr);
	}
	fpDestroySwapchainKHR(*deviceObject->getVkDevice(), swapChainPublicVariables.swapChain, nullptr);
	vkDestroySurfaceKHR(*appObject->getVulkanInstance()->getVkInstance(), swapChainPublicVariables.surface, nullptr);
}

VkResult VulkanSwapChain::createSwapChainExtensions() {
	VkInstance& instance = *appObject->getVulkanInstance()->getVkInstance();
	VkDevice& device = *appObject->getVulkanDevice()->getVkDevice();

	INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceSupportKHR);
	INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
	INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceFormatsKHR);
	INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfacePresentModesKHR);
	INSTANCE_FUNC_PTR(instance, DestroySurfaceKHR);
	
	DEVICE_FUNC_PTR(device, CreateSwapchainKHR);
	DEVICE_FUNC_PTR(device, DestroySwapchainKHR);
	DEVICE_FUNC_PTR(device, GetSwapchainImagesKHR);
	DEVICE_FUNC_PTR(device, AcquireNextImageKHR);
	DEVICE_FUNC_PTR(device, QueuePresentKHR);

	return VK_SUCCESS;
}

void VulkanSwapChain::getSupportedFormats() {
	VkPhysicalDevice* gpu = rendererObject->getDevice()->getPhysicalDevice();
	VkResult result;

	uint32_t formatCount;
	result = fpGetPhysicalDeviceSurfaceFormatsKHR(*gpu, swapChainPublicVariables.surface, &formatCount, nullptr);
	if (result != VK_SUCCESS) {
		std::cout << "[ ! ] : Failed to get surface format count.\n";
		return;
	}

	swapchainPrivateVariables.surfaceFormats.clear();
	swapchainPrivateVariables.surfaceFormats.resize(formatCount);

	result = fpGetPhysicalDeviceSurfaceFormatsKHR(*gpu, swapChainPublicVariables.surface, &formatCount, &swapchainPrivateVariables.surfaceFormats[0]);
	if (result != VK_SUCCESS) {
		std::cout << "[ ! ] : Failed to get surface format.\n";
		return;
	}

	if (formatCount == 1 && swapchainPrivateVariables.surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
		swapChainPublicVariables.format = VK_FORMAT_B8G8R8A8_UNORM;
	}
	else {
		assert(formatCount >= 1);
		swapChainPublicVariables.format = swapchainPrivateVariables.surfaceFormats[0].format;
	}
}

VkResult VulkanSwapChain::createSurface() {
	VkResult result;
	VkInstance& instance = *appObject->getVulkanInstance()->getVkInstance();

#ifdef _WIN32
	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.hinstance = rendererObject->connection;
	createInfo.hwnd = rendererObject->window;

	result = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &swapChainPublicVariables.surface);
#endif // _WIN32

	if (result != VK_SUCCESS) { std::cout << "[ ! ] Error : Failed to create surface.\n"; }

	return result;
}

uint32_t VulkanSwapChain::getGraphicsQueueWithPresentationSupport()
{
	VulkanDevice* device = appObject->getVulkanDevice();
	uint32_t queueCount = device->getQueueFamilyCount();
	VkPhysicalDevice gpu = *device->getPhysicalDevice();
	std::vector<VkQueueFamilyProperties>& queueProperties = *device->getQueueFamilyProperties();

	VkBool32* supportsPresent = (VkBool32*)malloc(queueCount * sizeof(VkBool32));
	for (uint32_t i = 0; i < queueCount; ++i) {
		fpGetPhysicalDeviceSurfaceSupportKHR(gpu, i, swapChainPublicVariables.surface, &supportsPresent[i]);
	}

	uint32_t graphicsQueueNodeIndex = UINT32_MAX;
	uint32_t presentQueueNodeIndex = UINT32_MAX;

	for (uint32_t i = 0; i < queueCount; ++i) {
		if ((queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
			if (graphicsQueueNodeIndex == UINT32_MAX)
				graphicsQueueNodeIndex = i;

			if (supportsPresent[i] == VK_TRUE) {
				graphicsQueueNodeIndex = i;
				presentQueueNodeIndex = i;
				break;
			}
		}
	}

	if (presentQueueNodeIndex == UINT32_MAX) {
		for (uint32_t i = 0; i < queueCount; ++i) {
			if (supportsPresent[i] == VK_TRUE) {
				presentQueueNodeIndex = i;
				break;
			}
		}
	}

	free(supportsPresent);

	if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX) { return UINT32_MAX; }

	return graphicsQueueNodeIndex;
}

void VulkanSwapChain::getSurfaceCapabilitiesAndPresentMode() {
	VkResult result;
	VkPhysicalDevice gpu = *appObject->getVulkanDevice()->getPhysicalDevice();
	result = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, swapChainPublicVariables.surface, &swapchainPrivateVariables.surfaceCapabilities);
	if (result != VK_SUCCESS) {
		std::cout << "[ ! ] : Failed to get surface capabilities.\n";
		return;
	}

	result = fpGetPhysicalDeviceSurfacePresentModesKHR(gpu, swapChainPublicVariables.surface, &swapchainPrivateVariables.presentModeCount, nullptr);
	assert(result == VK_SUCCESS);

	swapchainPrivateVariables.presentModes.clear();
	swapchainPrivateVariables.presentModes.resize(swapchainPrivateVariables.presentModeCount);
	assert(swapchainPrivateVariables.presentModeCount >= 1);

	result = fpGetPhysicalDeviceSurfacePresentModesKHR(gpu, swapChainPublicVariables.surface, &swapchainPrivateVariables.presentModeCount, &swapchainPrivateVariables.presentModes[0]);
	assert(result == VK_SUCCESS);

	if (swapchainPrivateVariables.surfaceCapabilities.currentExtent.width == static_cast<uint32_t>(-1)) {
		swapchainPrivateVariables.swapChainExtent.width = rendererObject->width;
		swapchainPrivateVariables.swapChainExtent.height = rendererObject->height;
	}
	else {
		swapchainPrivateVariables.swapChainExtent = swapchainPrivateVariables.surfaceCapabilities.currentExtent;
	}
}
