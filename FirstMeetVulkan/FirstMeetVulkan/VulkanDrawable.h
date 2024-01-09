#pragma once
#ifndef VULKAN_DRAWABLE_H
#define VULKAN_DRAWABLE_H

#include "Headers.h"

class VulkanRenderer;

class VulkanDrawable {
public:
	VulkanDrawable(VulkanRenderer* parent = 0);
	~VulkanDrawable();

public:
	void createVertexBuffer(const void* vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture);

	void prepare();
	void render();

	void destroyVertexBuffer();

private:
	void recordCommandBuffer(int currentBuffer, VkCommandBuffer* drawCommand);

public:
	struct {
		VkBuffer buffer;
		VkDeviceMemory memory;
		VkDescriptorBufferInfo bufferInfo;
	} VertexBuffer;

	VkVertexInputBindingDescription vertexInputBind = {};
	VkVertexInputAttributeDescription vertexInputAttribute[2] = {};

private:
	std::vector<VkCommandBuffer> drawCommands;
	VulkanRenderer* rendererObject;
};


#endif /* !VULKAN_DRAWABLE_H */
