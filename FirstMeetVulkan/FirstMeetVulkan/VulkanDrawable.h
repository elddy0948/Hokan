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
	void destroyVertexBuffer();

public:
	struct {
		VkBuffer buffer;
		VkDeviceMemory memory;
		VkDescriptorBufferInfo bufferInfo;
	} VertexBuffer;

	VkVertexInputBindingDescription vertexInputBind = {};
	VkVertexInputAttributeDescription vertexInputAttribute[2] = {};
	VulkanRenderer* rendererObject;
};


#endif /* !VULKAN_DRAWABLE_H */
