#include "VulkanApplication.h"

std::unique_ptr<VulkanApplication> VulkanApplication::app;
std::once_flag VulkanApplication::onlyOnce;

extern std::vector<const char*> instanceExtensionNames;
extern std::vector<const char*> layerNames;
extern std::vector<const char*> deviceExtensionNames;

VulkanApplication::VulkanApplication() {
	instance.initialize();
}

VulkanApplication::~VulkanApplication() {
	delete rendererObject;
	rendererObject = nullptr;
}

VulkanApplication* VulkanApplication::GetApp() {
	std::call_once(onlyOnce, []() {app.reset(new VulkanApplication()); });
	return app.get();
}

void VulkanApplication::initialize() {
	std::vector<VkPhysicalDevice> gpuList;

	instance.createInstance(layerNames, instanceExtensionNames, AppName);

#ifndef NDEBUG
	instance.setupDebugLayer();
#endif // !NDEBUG

	enumeratePhysicalDevices(gpuList);

	if (gpuList.size() > 0) {
		handShakeWithDevice(&gpuList[0], layerNames, deviceExtensionNames);
	}

	rendererObject = new VulkanRenderer(this, device);
	rendererObject->initialize();
}

VkResult VulkanApplication::enumeratePhysicalDevices(std::vector<VkPhysicalDevice>& devices)
{
	uint32_t deviceCount;
	VkResult result;

	result = vkEnumeratePhysicalDevices(*instance.getVkInstance(), &deviceCount, nullptr);

	if (result) { return result; }
	if (deviceCount == 0) { return VK_INCOMPLETE; }

	devices.resize(deviceCount);
	result = vkEnumeratePhysicalDevices(*instance.getVkInstance(), &deviceCount, devices.data());

	return  result;
}

VkResult VulkanApplication::handShakeWithDevice(VkPhysicalDevice* gpu, std::vector<const char*>& layers, std::vector<const char*>& extensions) {
	device = new VulkanDevice(gpu);
	return device->createLogicalDevice(layers, extensions);
}

/* TODO : Lifecycle of application */
void VulkanApplication::prepare() {}

void VulkanApplication::update() {}

bool VulkanApplication::render() {
	return rendererObject->render();
}

void VulkanApplication::deInitialize() {
	rendererObject->destroyDepthBuffer();
	rendererObject->getSwapChain()->destroySwapChain();
	rendererObject->destroyCommandBuffer();
	rendererObject->destroyCommandPool();
	rendererObject->destroyPresentationWindow();

	device->destroyDevice();
	instance.destroyInstance();
}