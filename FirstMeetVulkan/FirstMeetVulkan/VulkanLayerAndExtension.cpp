#include "VulkanLayerAndExtension.h"
#include "VulkanApplication.h"

VulkanLayerAndExtension::~VulkanLayerAndExtension() {
	dbgCreateDebugReportCallback = nullptr;
	dbgDestroyDebugReportCallback = nullptr;
	debugReportCallback = nullptr;
}

VkResult VulkanLayerAndExtension::fetchInstanceLayerProperties() {
	uint32_t layerCount;
	VkResult result;

	result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	if (result) 
		return result;

	if (layerCount == 0) 
		return VK_INCOMPLETE;

	layerProperties.resize(layerCount);
	result = vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

	return result;
}

VkResult VulkanLayerAndExtension::fetchInstanceExtensionProperties() {
	uint32_t extensionCount;
	VkResult result;

	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	if (result)
		return result;
	if (extensionCount == 0)
		return VK_INCOMPLETE;

	extensionProperties.resize(extensionCount);
	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());

	return result;
}

VkResult VulkanLayerAndExtension::fetchDeviceExtensionProperties(VkPhysicalDevice* physicalDevice) {
	uint32_t extensionCount;
	VkResult result;

	result = vkEnumerateDeviceExtensionProperties(*physicalDevice, nullptr, &extensionCount, nullptr);

	if (result)
		return result;
	if (extensionCount == 0)
		return VK_INCOMPLETE;

	physicalDeviceExtensionProperties.resize(extensionCount);
	result = vkEnumerateDeviceExtensionProperties(*physicalDevice, nullptr, &extensionCount, physicalDeviceExtensionProperties.data());

	return result;
}

std::vector<const char*> VulkanLayerAndExtension::fetchEnableLayers(std::vector<const char*>& desiredLayers)
{
	uint32_t numberOfAvailable = static_cast<uint32_t>(layerProperties.size());
	uint32_t numberOfDesired = static_cast<uint32_t>(desiredLayers.size());
	std::vector<const char*> enableLayers;

	for (uint32_t i = 0; i < numberOfDesired; ++i) {
		for (uint32_t j = 0; j < numberOfAvailable; ++j) {
			if (strcmp(desiredLayers[i], layerProperties[j].layerName) == 0) {
				enableLayers.push_back(desiredLayers[i]);
				break;
			}
		}
	}

	return enableLayers;
}

std::vector<const char*> VulkanLayerAndExtension::fetchInstanceEnableExtensions(std::vector<const char*>& desiredExtensions) {
	uint32_t numberOfAvailable = static_cast<uint32_t>(extensionProperties.size());
	uint32_t numberOfDesired = static_cast<uint32_t>(desiredExtensions.size());
	std::vector<const char*> enableExtensions;

	for (uint32_t i = 0; i < numberOfDesired; ++i) {
		for (uint32_t j = 0; j < numberOfAvailable; ++j) {
			if (strcmp(desiredExtensions[i], extensionProperties[j].extensionName) == 0) {
				enableExtensions.push_back(desiredExtensions[i]);
				break;
			}
		}
	}

	return enableExtensions;
}

std::vector<const char*> VulkanLayerAndExtension::fetchDeviceEnableExtensions(std::vector<const char*>& desiredExtensions) {
	uint32_t numberOfAvailable = static_cast<uint32_t>(physicalDeviceExtensionProperties.size());
	uint32_t numberOfDesired = static_cast<uint32_t>(desiredExtensions.size());
	std::vector<const char*> enableExtensions;

	for (uint32_t i = 0; i < numberOfDesired; ++i) {
		for (uint32_t j = 0; j < numberOfAvailable; ++j) {
			if (strcmp(desiredExtensions[i], physicalDeviceExtensionProperties[j].extensionName) == 0) {
				enableExtensions.push_back(desiredExtensions[i]);
				break;
			}
		}
	}

	return enableExtensions;
}

/* Debug Layer */
VkResult VulkanLayerAndExtension::createDebugReportCallback()
{
	VkResult result;
	VulkanApplication* app = VulkanApplication::GetApp();
	VkInstance* instance = app->getVulkanInstance()->getVkInstance();

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