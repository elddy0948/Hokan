#include "VulkanLayerAndExtension.h"
#include "VulkanApplication.h"

VulkanLayerAndExtension::VulkanLayerAndExtension() {
}

VulkanLayerAndExtension::~VulkanLayerAndExtension() {
	dbgCreateDebugReportCallback = nullptr;
	dbgDestroyDebugReportCallback = nullptr;
	debugReportCallback = nullptr;
}

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
	VkResult result = VK_INCOMPLETE;

	std::cout << "\nDevice extensions\n";
	std::cout << "============================================================\n";

	VulkanApplication* app = VulkanApplication::GetApp();
	std::vector<LayerProperties>* instanceLayerProperties = &app->getVulkanInstance()->getLayerExtension()->layerPropertyList;

	for (auto globalLayerProperty : *instanceLayerProperties) {
		LayerProperties layerProperties;
		layerProperties.properties = globalLayerProperty.properties;
		if (result = getExtensionProperties(layerProperties, physicalDevice))
			continue;
	
		std::cout << '\n' << globalLayerProperty.properties.description << "\n\t|\n\t|---[Layer Name]--> " << globalLayerProperty.properties.layerName << '\n';
		layerPropertyList.push_back(layerProperties);

		if (layerProperties.extensions.size()) {
			for (const auto& extension : layerProperties.extensions) {
				std::cout << "\n\t\t|\n\t\t|---[Device Extension]--> " << extension.extensionName << '\n';
			}
		}
		else {
			std::cout << "\n\t\t|\n\t\t|---[Device Extension]--> No Extension Found.\n";
		}
	}

	return result;
}

VkResult VulkanLayerAndExtension::createDebugReportCallback()
{
	VkResult result;
	VulkanApplication* app = VulkanApplication::GetApp();
	VkInstance* instance = app->getVulkanInstance()->getVkInstance();

	// ...

	dbgCreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(*instance, "vkCreateDebugReportCallbackEXT");
	if (!dbgCreateDebugReportCallback) {
		std::cout << "Error : GetInstanceProcAddr unable to locate vkCreateDebugReportCallbackEXT function.\n";
		return VK_ERROR_INITIALIZATION_FAILED;
	}
	std::cout << "GetInstanceProcAddr loaded dbgCreateDebugReportCallback function\n";

	dbgDestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(*instance, "vkDestroyDebugReportCallbackEXT");
	if (!dbgDestroyDebugReportCallback) {
		std::cout << "Error : GetInstanceProcAddr unable to locate vkDestroyDebugReportCallbackEXT function.\n";
		return VK_ERROR_INITIALIZATION_FAILED;
	}
	std::cout << "GetInstanceProcAddr loaded dbgDestroyDebugReportCallback function\n";

	dbgReportCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	dbgReportCreateInfo.pfnCallback = debugFunction;
	dbgReportCreateInfo.pUserData = nullptr;
	dbgReportCreateInfo.pNext = nullptr;
	dbgReportCreateInfo.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_ERROR_BIT_EXT |
		VK_DEBUG_REPORT_DEBUG_BIT_EXT;

	result = dbgCreateDebugReportCallback(*instance, &dbgReportCreateInfo, nullptr, &debugReportCallback);

	if (result == VK_SUCCESS) {
		std::cout << "Debug report callback object created successfully\n";
	}

	return result;
}

void VulkanLayerAndExtension::destroyDebugReportCallback() {
	VulkanApplication* app = VulkanApplication::GetApp();
	VkInstance& instance = *app->getVulkanInstance()->getVkInstance();
	dbgDestroyDebugReportCallback(instance, debugReportCallback, nullptr);
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanLayerAndExtension::debugFunction(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location, int32_t msgCode, const char* layerPrefix, const char* msg, void* userData) {
	if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
		std::cout << "[VK_DEBUG_REPORT] ERROR : [" << layerPrefix << "] Code " << msgCode << ":" << msg << '\n';
	}
	else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
		std::cout << "[VK_DEBUG_REPORT] WARNING : [" << layerPrefix << "] Code " << msgCode << ":" << msg << '\n';
	}
	else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
		std::cout << "[VK_DEBUG_REPORT] INFORMATION : [" << layerPrefix << "] Code " << msgCode << ":" << msg << '\n';
	}
	else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
		std::cout << "[VK_DEBUG_REPORT] PERFORMANCE : [" << layerPrefix << "] Code " << msgCode << ":" << msg << '\n';
	}
	else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
		std::cout << "[VK_DEBUG_REPORT] DEBUG : [" << layerPrefix << "] Code " << msgCode << ":" << msg << '\n';
	}
	else {
		return VK_FALSE;
	}

	fflush(stdout);
	return VK_TRUE;
}

VkBool32 VulkanLayerAndExtension::areLayersSupported(std::vector<const char*>& layerNames)
{
	uint32_t checkCount = static_cast<uint32_t>(layerNames.size());
	uint32_t layerCount = static_cast<uint32_t>(layerPropertyList.size());
	std::vector<const char*> unsupportedLayerNames;

	for (uint32_t i = 0; i < checkCount; ++i) {
		VkBool32 isSupported = 0;
		for (uint32_t j = 0; j < layerCount; ++j) {
			if (!strcmp(layerNames[i], layerPropertyList[j].properties.layerName)) { isSupported = 1; }
		}

		if (!isSupported) {
			std::cout << "No layer support found, removed from layer: " << layerNames[i] << '\n';
			unsupportedLayerNames.push_back(layerNames[i]);
		}
		else {
			std::cout << "Layer supported: " << layerNames[i] << '\n';
		}
	}

	for (const auto& layer : unsupportedLayerNames) {
		auto pos = std::find(layerNames.begin(), layerNames.end(), layer);
		if (pos != layerNames.end()) 
			layerNames.erase(pos);
	}
	
	return true;
}
