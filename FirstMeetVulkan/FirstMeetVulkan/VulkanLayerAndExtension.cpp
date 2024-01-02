#include "VulkanLayerAndExtension.h"

VkResult VulkanLayerAndExtension::getInstanceLayerProperties() {
	uint32_t instanceLayerCount;
	std::vector<VkLayerProperties> layerProperties;
	VkResult result;

	do {
		result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);

		if (result)
			return result;
		if (instanceLayerCount == 0)
			return VK_INCOMPLETE;

		layerProperties.resize(instanceLayerCount);
		result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerProperties.data());
	} while (result == VK_INCOMPLETE);

	std::cout << "\nInstanced Layers\n";
	std::cout << "============================================================\n";

	for (const auto& globalLayerProperty : layerProperties) {
		std::cout << '\n' << globalLayerProperty.description << "\n\t|\n\t|---[Layer Name]-> " << globalLayerProperty.layerName << '\n';

		LayerProperties lp;
		lp.properties = globalLayerProperty;

		result = getExtensionProperties(lp);

		if (result)
			continue;

		layerPropertyList.push_back(lp);

		for (const auto& extension : lp.extensions) {
			std::cout << "\t\t|\n\t\t-[Layer Extension]-> " << extension.extensionName << '\n';
		}
	}

	return result;
}

VkResult VulkanLayerAndExtension::getExtensionProperties(LayerProperties& layerProperties, VkPhysicalDevice* physicalDevice) {
	uint32_t extensionCount;
	std::vector<VkExtensionProperties> extensions;
	VkResult result;

	do {
		if (physicalDevice)
			result = vkEnumerateDeviceExtensionProperties(*physicalDevice, layerProperties.properties.layerName, &extensionCount, nullptr);
		else
			result = vkEnumerateInstanceExtensionProperties(layerProperties.properties.layerName, &extensionCount, nullptr);


		if (result) { return result; }
		if (extensionCount == 0) { return VK_INCOMPLETE; }

		extensions.resize(extensionCount);

		if (physicalDevice)
			result = vkEnumerateDeviceExtensionProperties(*physicalDevice, layerProperties.properties.layerName, &extensionCount, extensions.data());
		else
			result = vkEnumerateInstanceExtensionProperties(layerProperties.properties.layerName, &extensionCount, extensions.data());
	} while (result == VK_INCOMPLETE);

	layerProperties.extensions = extensions;

	return result;
}

VkResult VulkanLayerAndExtension::getDeviceExtensionProperties(VkPhysicalDevice* physicalDevice) {
	return VK_INCOMPLETE;
}
