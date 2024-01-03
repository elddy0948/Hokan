#include "VulkanApplication.h"

std::unique_ptr<VulkanApplication> VulkanApplication::app;
std::once_flag VulkanApplication::onlyOnce;

extern std::vector<const char*> instanceExtensionNames;
extern std::vector<const char*> layerNames;
extern std::vector<const char*> deviceExtensionNames;	// TODO: set up device extension names

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

	std::vector<VkPhysicalDevice> gpuList;
	enumeratePhysicalDevices(gpuList);

	if (gpuList.size() > 0) {
		handShakeWithDevice(&gpuList[0], layerNames, deviceExtensionNames);
	}
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
void VulkanApplication::render() {}
void VulkanApplication::deInitialize() {}