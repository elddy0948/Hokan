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