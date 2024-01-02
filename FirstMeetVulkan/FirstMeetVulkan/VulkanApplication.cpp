#include "VulkanApplication.h"

std::unique_ptr<VulkanApplication> VulkanApplication::app;
std::once_flag VulkanApplication::onlyOnce;

VulkanApplication::VulkanApplication() {
	instance.getLayerExtension()->getInstanceLayerProperties();
}

VulkanApplication::~VulkanApplication() {
}

VulkanApplication* VulkanApplication::GetApp() {
	std::call_once(onlyOnce, []() {app.reset(new VulkanApplication()); });
	return app.get();
}

VkResult VulkanApplication::createVulkanInstance(std::vector<const char*> layers, std::vector<const char*> extensions) {
	return instance.createInstance(layers, extensions, AppName);
}

