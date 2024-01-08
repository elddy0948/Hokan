#include "VulkanInstance.h"

void VulkanInstance::initialize() {
	layerExtension.fetchInstanceLayerProperties();
	layerExtension.fetchInstanceExtensionProperties();
}

VkResult VulkanInstance::createInstance(std::vector<const char*>& layers, std::vector<const char*>& extensions, const char* appName) {
	VkApplicationInfo appInfo{};
	VkInstanceCreateInfo createInfo{}; 
	VkResult result;

	std::vector<const char*> enableLayers = layerExtension.fetchEnableLayers(layers);
	std::vector<const char*> enableExtensions = layerExtension.fetchInstanceEnableExtensions(extensions);

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = appName;
	appInfo.applicationVersion = 1;
	appInfo.pEngineName = appName;
	appInfo.engineVersion = 1;
	appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = layerExtension.getDbgReportCreateInfo();
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = static_cast<uint32_t>(enableLayers.size());
	createInfo.ppEnabledLayerNames = enableLayers.size() ? enableLayers.data() : nullptr;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(enableExtensions.size());
	createInfo.ppEnabledExtensionNames = enableExtensions.size() ? enableExtensions.data() : nullptr;

	result = vkCreateInstance(&createInfo, nullptr, &instance);

	return result;
}

void VulkanInstance::setupDebugLayer() {
	layerExtension.createDebugReportCallback();
}

void VulkanInstance::destroyInstance() {
#ifndef NDEBUG
	layerExtension.destroyDebugReportCallback();
#endif // NDEBUG

	vkDestroyInstance(instance, nullptr);
}
