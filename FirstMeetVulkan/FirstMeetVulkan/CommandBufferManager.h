#pragma once
#ifndef COMMAND_BUFFER_MANAGER_H
#define COMMAND_BUFFER_MANAGER_H

#include "Headers.h"

class CommandBufferManager {
public:
	static void allocCommandBuffer(const VkDevice* device, const VkCommandPool commandPool, VkCommandBuffer* commandBuffer, const VkCommandBufferAllocateInfo* commandBufferInfo);
	static void beginCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferBeginInfo* beginInfo = nullptr);
	static void endCommandBuffer(VkCommandBuffer commandBuffer);
	static void submitCommandBuffer(const VkQueue& queue, const VkCommandBuffer* commandBufferList, const VkSubmitInfo* submitInfo = nullptr, const VkFence& fence = VK_NULL_HANDLE);
};


#endif /* !COMMAND_BUFFER_MANAGER_H */
