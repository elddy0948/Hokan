#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "VulkanApplication.h"

VulkanDevice::~VulkanDevice() {}

VkResult VulkanDevice::createDevice(std::vector<const char*> layers, std::vector<const char*> extensions) {
	layerExtension.setLayerNames(layers);
	layerExtension.setExtensionNames(extensions);

	VkResult result;
	float queuePriorities[1] = { 0.0 };

	VkDeviceQueueCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
	createInfo.queueCount = 1;
	createInfo.pQueuePriorities = queuePriorities;

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &createInfo;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = extensions.size() ? extensions.data() : nullptr;
	deviceCreateInfo.pEnabledFeatures = nullptr;

	result = vkCreateDevice(*physicalDevice, &deviceCreateInfo, nullptr, &device);
	return result;
}

void VulkanDevice::getPhysicalDeviceQueuesAndProperties() {
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueFamilyCount, nullptr);
	queueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
}

bool VulkanDevice::getGraphicsQueueHandle() {
	bool found = false;
	for (unsigned int i = 0; i < queueFamilyCount; ++i) {
		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			found = true;
			graphicsQueueFamilyIndex = i;
			break;
		}
	}
	return found;
}

void VulkanDevice::getDeviceQueue() {
	vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &queue);
}

void VulkanDevice::destroyDevice() {
	vkDestroyDevice(device, nullptr);
}

bool VulkanDevice::memoryTypeFromProperties(uint32_t typeBits, VkFlags requirementsMask, uint32_t* typeIndex)
{
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
