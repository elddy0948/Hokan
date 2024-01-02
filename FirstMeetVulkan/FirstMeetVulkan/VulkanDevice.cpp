#include "VulkanDevice.h"

VulkanDevice::~VulkanDevice() {}

VkResult VulkanDevice::createDevice(std::vector<const char*> layers, std::vector<const char*> extensions) {
	layerExtension.setLayerNames(layers);
	layerExtension.setExtensionNames(extensions);

	VkResult result;
	float queuePriority[1] = { 0.0 };

	VkDeviceQueueCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
	createInfo.queueCount = 1;
	createInfo.pQueuePriorities = queuePriority;

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &createInfo;
	deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
	deviceCreateInfo.ppEnabledLayerNames = layers.data();
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
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

void VulkanDevice::destroyDevice() {
	vkDestroyDevice(device, nullptr);
}
