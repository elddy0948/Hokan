#include "CommandBufferManager.h"

void CommandBufferManager::allocCommandBuffer(const VkDevice* device, const VkCommandPool commandPool, VkCommandBuffer* commandBuffer, const VkCommandBufferAllocateInfo* commandBufferInfo) {
	VkResult result;

	if (commandBufferInfo) {
		result = vkAllocateCommandBuffers(*device, commandBufferInfo, commandBuffer);
		return;
	}

	VkCommandBufferAllocateInfo commandInfo = {};
	
	commandInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandInfo.pNext = nullptr;
	commandInfo.commandPool = commandPool;
	commandInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandInfo.commandBufferCount = static_cast<uint32_t>(sizeof(commandBuffer) / sizeof(VkCommandBuffer));
	
	result = vkAllocateCommandBuffers(*device, &commandInfo, commandBuffer);
}

void CommandBufferManager::beginCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferBeginInfo* beginInfo) {
	VkResult result;
	if (beginInfo) {
		result = vkBeginCommandBuffer(commandBuffer, beginInfo);
		if (result != VK_SUCCESS) { std::cout << "[ ! ] : Failed to begin command buffer.\n"; }
		return;
	}

	VkCommandBufferInheritanceInfo inheritanceInfo = {};
	inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	inheritanceInfo.pNext = nullptr;
	inheritanceInfo.renderPass = VK_NULL_HANDLE;
	inheritanceInfo.subpass = 0;
	inheritanceInfo.framebuffer = VK_NULL_HANDLE;
	inheritanceInfo.occlusionQueryEnable = VK_FALSE;
	inheritanceInfo.queryFlags = 0;
	inheritanceInfo.pipelineStatistics = 0;

	VkCommandBufferBeginInfo defaultBeginInfo = {};
	
	defaultBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	defaultBeginInfo.pNext = nullptr;
	defaultBeginInfo.flags = 0;
	defaultBeginInfo.pInheritanceInfo = &inheritanceInfo;

	result = vkBeginCommandBuffer(commandBuffer, &defaultBeginInfo);

	if (result != VK_SUCCESS) { std::cout << "[ ! ] : Failed to begin command buffer.\n"; }
}

void CommandBufferManager::endCommandBuffer(VkCommandBuffer commandBuffer) {
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		std::cout << "[ ! ] : Failed to end command buffer.\n";
	}
}

void CommandBufferManager::submitCommandBuffer(const VkQueue& queue, const VkCommandBuffer* commandBufferList, const VkSubmitInfo* submitInfo, const VkFence& fence) {
	VkResult result;
	if (submitInfo) {
		result = vkQueueSubmit(queue, 1, submitInfo, fence);
		if (!result) { std::cout << " [ ! ] : Failed to submit queue.\n"; }
		result = vkQueueWaitIdle(queue);
		if (!result) { std::cout << "[ ! ] : Something went wrong in waiting queue.\n"; }
		return;
	}

	VkSubmitInfo defaultSubmitInfo = {};
	defaultSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	defaultSubmitInfo.pNext = nullptr;
	defaultSubmitInfo.waitSemaphoreCount = 0;
	defaultSubmitInfo.pWaitSemaphores = nullptr;
	defaultSubmitInfo.pWaitDstStageMask = nullptr;
	defaultSubmitInfo.commandBufferCount = static_cast<uint32_t>(sizeof(commandBufferList) / sizeof(VkCommandBuffer));
	defaultSubmitInfo.pCommandBuffers = commandBufferList;
	defaultSubmitInfo.signalSemaphoreCount = 0;
	defaultSubmitInfo.pSignalSemaphores = nullptr;

	result = vkQueueSubmit(queue, 1, &defaultSubmitInfo, fence);
	if (!result) { std::cout << " [ ! ] : Failed to submit queue.\n"; }
	result = vkQueueWaitIdle(queue);
	if (!result) { std::cout << "[ ! ] : Something went wrong in waiting queue.\n"; }
}
