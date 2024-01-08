#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "VulkanApplication.h"

VulkanDevice::VulkanDevice(VkPhysicalDevice* physicalDevice) : physicalDevice(physicalDevice) {
	layerExtension.fetchDeviceExtensionProperties(this->physicalDevice);
	vkGetPhysicalDeviceProperties(*this->physicalDevice, &deviceProperties);
	vkGetPhysicalDeviceMemoryProperties(*this->physicalDevice, &memoryProperties);

	fetchQueuesAndProperties();
	fetchGraphicsQueueHandle();
}

void VulkanDevice::fetchQueuesAndProperties() {
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueFamilyCount, nullptr);
	queueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
}

void VulkanDevice::fetchGraphicsQueueHandle() {
	for (uint32_t i = 0; i < queueFamilyCount; ++i) {
		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			graphicsQueueFamilyIndex = i;
			break;
		}
	}
}

VkResult VulkanDevice::createLogicalDevice(std::vector<const char*> layers, std::vector<const char*> extensions) {
	VkResult result;
	float queuePriorities[1] = { 0.0 };
	VkDeviceQueueCreateInfo createInfo = {};
	VkDeviceCreateInfo deviceCreateInfo = {};
	std::vector<const char*> enableExtensions = layerExtension.fetchDeviceEnableExtensions(extensions);

	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
	createInfo.queueCount = 1;
	createInfo.pQueuePriorities = queuePriorities;

	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &createInfo;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enableExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = enableExtensions.size() ? enableExtensions.data() : nullptr;
	deviceCreateInfo.pEnabledFeatures = nullptr;

	result = vkCreateDevice(*physicalDevice, &deviceCreateInfo, nullptr, &device);
	 
	return result;
}

bool VulkanDevice::memoryTypeFromProperties(uint32_t typeBits, VkFlags requirementsMask, uint32_t* typeIndex) {
	for (uint32_t i = 0; i < 32; ++i) {
		if ((typeBits & 1) == 1) {
			if ((memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask) {
				*typeIndex = i;
				return true;
			}
		}
		typeBits >>= 1;
	}
	return false;
}
