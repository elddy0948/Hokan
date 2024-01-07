#include "VulkanApplication.h"

std::unique_ptr<VulkanApplication> VulkanApplication::app;
std::once_flag VulkanApplication::onlyOnce;

extern std::vector<const char*> instanceExtensionNames;
extern std::vector<const char*> layerNames;
extern std::vector<const char*> deviceExtensionNames;

VulkanApplication::VulkanApplication() {
	instance.getLayerExtension()->getInstanceLayerProperties();

	device = nullptr;
	debugFlag = false;
	rendererObject = nullptr;
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
#ifndef NDEBUG
	instance.getLayerExtension()->areLayersSupported(layerNames);
#endif // !NDEBUG

	instance.createInstance(layerNames, instanceExtensionNames, AppName);

#ifndef NDEBUG
	instance.getLayerExtension()->createDebugReportCallback();
#endif // !NDEBUG

	std::vector<VkPhysicalDevice> gpuList;
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

VkResult VulkanApplication::handShakeWithDevice(VkPhysicalDevice* gpu, std::vector<const char*>& layers, std::vector<const char*>& extensions)
{
	device = new VulkanDevice(gpu);

	device->getLayerExtension()->getDeviceExtensionProperties(gpu);
	
	vkGetPhysicalDeviceProperties(*gpu, device->getPhysicalDeviceProperties());
	vkGetPhysicalDeviceMemoryProperties(*gpu, device->getMemoryProperties());

	device->getPhysicalDeviceQueuesAndProperties();
	device->getGraphicsQueueHandle();

	return device->createDevice(layers, extensions);
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
#ifndef NDEBUG
	instance.getLayerExtension()->destroyDebugReportCallback();
#endif // !NDEBUG
	instance.destroyInstance();
}