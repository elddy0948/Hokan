#include "VulkanInstance.h"

VulkanInstance::~VulkanInstance() {}

VkResult VulkanInstance::createInstance(std::vector<const char*>& layers, std::vector<const char*>& extensions, const char* appName) {
	VkApplicationInfo appInfo{};
	VkInstanceCreateInfo createInfo{}; 
	VkResult result;

	layerExtension.setLayerNames(layers);
	layerExtension.setExtensionNames(extensions);

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = appName;
	appInfo.applicationVersion = 1;
	appInfo.pEngineName = appName;
	appInfo.engineVersion = 1;
	appInfo.apiVersion = VK_API_VERSION_1_0;

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = VK_NULL_HANDLE;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
	createInfo.ppEnabledLayerNames = layers.size() ? layers.data() : nullptr;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.size() ? extensions.data() : nullptr;

	result = vkCreateInstance(&createInfo, nullptr, &instance);

	return result;
}

void VulkanInstance::destroyInstance() {
	vkDestroyInstance(instance, nullptr);
}
