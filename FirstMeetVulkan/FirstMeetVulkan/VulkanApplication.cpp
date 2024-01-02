#include "VulkanApplication.h"

std::unique_ptr<VulkanApplication> VulkanApplication::app;
std::once_flag VulkanApplication::onlyOnce;

extern std::vector<const char*> instanceExtensionNames;
extern std::vector<const char*> layerNames;

VulkanApplication::VulkanApplication() {
	instance.getLayerExtension()->getInstanceLayerProperties();
}

VulkanApplication::~VulkanApplication() {
}

VulkanApplication* VulkanApplication::GetApp() {
	std::call_once(onlyOnce, []() {app.reset(new VulkanApplication()); });
	return app.get();
}

void VulkanApplication::initialize() {
	instance.createInstance(layerNames, instanceExtensionNames, AppName);
}

VkResult VulkanApplication::enumeratePhysicalDevices(std::vector<VkPhysicalDevice>& devices)
{
	uint32_t deviceCount;
	VkResult result;

	result = vkEnumeratePhysicalDevices(*instance.getInstance(), &deviceCount, nullptr);

	if (result) { return result; }
	if (deviceCount == 0) { return VK_INCOMPLETE; }

	devices.resize(deviceCount);
	result = vkEnumeratePhysicalDevices(*instance.getInstance(), &deviceCount, devices.data());
	return  result;
}