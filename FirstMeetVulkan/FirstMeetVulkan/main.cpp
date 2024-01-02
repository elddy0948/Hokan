#include "Headers.h"
#include "VulkanApplication.h"

std::vector<const char*> instanceExtensionNames = {
	VK_KHR_SURFACE_EXTENSION_NAME,
};

std::vector<const char*> layerNames = {
	"VK_LAYER_LUNARG_api_dump",
};

int main(void) {
	VulkanApplication* app = VulkanApplication::GetApp();
	std::vector<VkPhysicalDevice> gpus;

	app->initialize();

	app->enumeratePhysicalDevices(gpus);
	app->getInstance()->getLayerExtension()->getDeviceExtensionProperties(&gpus[0]);
	while (true) {}
	return 0;
}