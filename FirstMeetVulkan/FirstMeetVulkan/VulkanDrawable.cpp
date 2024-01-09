#include "VulkanDrawable.h"

#include "VulkanApplication.h"
#include "VulkanRenderer.h"

VulkanDrawable::VulkanDrawable(VulkanRenderer* parent) : rendererObject(parent) {
	memset(&VertexBuffer, 0, sizeof(VertexBuffer));
}

VulkanDrawable::~VulkanDrawable() {}

void VulkanDrawable::createVertexBuffer(const void* vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture) {
	VulkanApplication* appObject = VulkanApplication::GetApp();
	VulkanDevice* deviceObject = appObject->getVulkanDevice();
	VkResult result;
	bool pass;
	VkBufferCreateInfo bufferInfo = {};
	VkMemoryRequirements memoryRequirements;
	VkMemoryAllocateInfo allocateInfo = {};
	uint8_t* pData;

	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = nullptr;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.size = dataSize;
	bufferInfo.queueFamilyIndexCount = 0;
	bufferInfo.pQueueFamilyIndices = nullptr;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.flags = 0;

	result = vkCreateBuffer(*deviceObject->getVkDevice(), &bufferInfo, nullptr, &VertexBuffer.buffer);
	assert(result == VK_SUCCESS);

	vkGetBufferMemoryRequirements(*deviceObject->getVkDevice(), VertexBuffer.buffer, &memoryRequirements);

	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.pNext = nullptr;
	allocateInfo.memoryTypeIndex = 0;
	allocateInfo.allocationSize = memoryRequirements.size;

	pass = deviceObject->memoryTypeFromProperties(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &allocateInfo.memoryTypeIndex);
	assert(pass);

	result = vkAllocateMemory(*deviceObject->getVkDevice(), &allocateInfo, nullptr, &(VertexBuffer.memory));
	assert(result == VK_SUCCESS);


	VertexBuffer.bufferInfo.range = memoryRequirements.size;
	VertexBuffer.bufferInfo.offset = 0;

	result = vkMapMemory(*deviceObject->getVkDevice(), VertexBuffer.memory, 0, memoryRequirements.size, 0, (void**)&pData);
	assert(result == VK_SUCCESS);

	memcpy(pData, vertexData, dataSize);

	vkUnmapMemory(*deviceObject->getVkDevice(), VertexBuffer.memory);

	result = vkBindBufferMemory(*deviceObject->getVkDevice(), VertexBuffer.buffer, VertexBuffer.memory, 0);
	assert(result == VK_SUCCESS);

	vertexInputBind.binding = 0;
	vertexInputBind.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertexInputBind.stride = dataStride;

	vertexInputAttribute[0].binding = 0;
	vertexInputAttribute[0].location = 0;
	vertexInputAttribute[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	vertexInputAttribute[0].offset = 0;
	vertexInputAttribute[1].binding = 0;
	vertexInputAttribute[1].location = 1;
	vertexInputAttribute[1].format = useTexture ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
	vertexInputAttribute[1].offset = 16;
}

void VulkanDrawable::destroyVertexBuffer() {
	VulkanDevice* deviceObject = rendererObject->getDevice();
	vkDestroyBuffer(*deviceObject->getVkDevice(), VertexBuffer.buffer, nullptr);
	vkFreeMemory(*deviceObject->getVkDevice(), VertexBuffer.memory, nullptr);
}
