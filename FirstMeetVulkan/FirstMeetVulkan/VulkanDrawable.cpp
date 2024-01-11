#include "VulkanDrawable.h"

#include "VulkanApplication.h"
#include "VulkanRenderer.h"
#include "CommandBufferManager.h"

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


void VulkanDrawable::initViewports(VkCommandBuffer* commandBuffer) {
	viewport.height = static_cast<float>(rendererObject->height);
	viewport.width = static_cast<float>(rendererObject->width);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	viewport.x = 0;
	viewport.y = 0;

	vkCmdSetViewport(*commandBuffer, 0, NUMBER_OF_VIEWPORTS, &viewport);
}


void VulkanDrawable::initScissors(VkCommandBuffer* commandBuffer) {
	scissor.extent.width = rendererObject->height;
	scissor.extent.width = rendererObject->width;
	scissor.offset.x = 0;
	scissor.offset.y = 0;

	vkCmdSetScissor(*commandBuffer, 0, NUMBER_OF_SCISSORS, &scissor);
}

void VulkanDrawable::prepare() {
	VulkanDevice* deviceObject = rendererObject->getDevice();

	drawCommands.resize(rendererObject->getSwapchain()->swapChainPublicVariables.colorBuffer.size());

	for (int i = 0; i < rendererObject->getSwapchain()->swapChainPublicVariables.colorBuffer.size(); ++i) {
		CommandBufferManager::allocCommandBuffer(deviceObject->getVkDevice(), *rendererObject->getCommandPool() , &drawCommands[i]);
		CommandBufferManager::beginCommandBuffer(drawCommands[i]);

		recordCommandBuffer(i, &drawCommands[i]);

		CommandBufferManager::endCommandBuffer(drawCommands[i]);
	}

}

void VulkanDrawable::render() {
	VulkanDevice* deviceObject = rendererObject->getDevice();
	VulkanSwapChain* swapchainObject = rendererObject->getSwapchain();

	uint32_t& currentColorImage = swapchainObject->swapChainPublicVariables.currentColorBuffer;
	VkSwapchainKHR& swapchain = swapchainObject->swapChainPublicVariables.swapChain;
	VkSemaphore presentCompleteSemaphore;
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};

	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;
	vkCreateSemaphore(*deviceObject->getVkDevice(), &semaphoreCreateInfo, nullptr, &presentCompleteSemaphore);

	Sleep(1000);

	VkResult result = swapchainObject->fpAcquireNextImageKHR(*deviceObject->getVkDevice(), swapchain, UINT64_MAX, presentCompleteSemaphore, VK_NULL_HANDLE, &currentColorImage);

	CommandBufferManager::submitCommandBuffer(*deviceObject->getQueue(), &drawCommands[currentColorImage], nullptr);

	VkPresentInfoKHR present = {};
	present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present.swapchainCount = 1;
	present.pSwapchains = &swapchain;
	present.pImageIndices = &currentColorImage;

	result = swapchainObject->fpQueuePresentKHR(*deviceObject->getQueue(), &present);
	assert(result == VK_SUCCESS);

	vkDestroySemaphore(*deviceObject->getVkDevice(), presentCompleteSemaphore, nullptr);
}

void VulkanDrawable::destroyVertexBuffer() {
	VulkanDevice* deviceObject = rendererObject->getDevice();
	vkDestroyBuffer(*deviceObject->getVkDevice(), VertexBuffer.buffer, nullptr);
	vkFreeMemory(*deviceObject->getVkDevice(), VertexBuffer.memory, nullptr);
}

void VulkanDrawable::recordCommandBuffer(int currentBuffer, VkCommandBuffer* drawCommand) {
	VulkanDevice* deviceObject = rendererObject->getDevice();

	VkClearValue clearValues[2];

	clearValues[0].color.float32[0] = 0.0f;
	clearValues[0].color.float32[1] = 0.0f;
	clearValues[0].color.float32[2] = 0.0f;
	clearValues[0].color.float32[3] = 0.0f;

	clearValues[1].depthStencil.depth = 1.0f;
	clearValues[1].depthStencil.stencil = 0;

	VkRenderPassBeginInfo renderPassBegin = {};
	renderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBegin.pNext = nullptr;
	renderPassBegin.renderPass = rendererObject->renderPass;
	renderPassBegin.framebuffer = rendererObject->framebuffers[currentBuffer];
	renderPassBegin.renderArea.offset.x = 0;
	renderPassBegin.renderArea.offset.y = 0;
	renderPassBegin.renderArea.extent.width = rendererObject->width;
	renderPassBegin.renderArea.extent.height = rendererObject->height;
	renderPassBegin.clearValueCount = 2;
	renderPassBegin.pClearValues = clearValues;

	vkCmdBeginRenderPass(*drawCommand, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);

	// Commands

	/* Bind command to graphics pipeline */
	vkCmdBindPipeline(*drawCommand, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);
	
	/* Geometry Data */
	const VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(&drawCommand, 0, 1, &VertexBuffer.buffer, offsets);

	/* Viewport */
	initViewports(drawCommand);

	/* Scissor */
	initScissors(drawCommand);

	vkCmdDraw(*drawCommand, 3, 1, 0, 0);

	vkCmdEndRenderPass(*drawCommand);
}