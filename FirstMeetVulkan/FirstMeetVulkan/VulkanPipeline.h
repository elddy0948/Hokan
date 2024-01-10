#pragma once
#ifndef VULKAN_PIPELINE_H
#define VULKAN_PIPELINE_H

#include "Headers.h"

class VulkanShader;
class VulkanDrawable;
class VulkanDevice;
class VulkanApplication;

#define NUMBER_OF_VIEWPORTS 1
#define NUMBER_OF_SCISSORS NUMBER_OF_VIEWPORTS

class VulkanPipeline {
public:
	VulkanPipeline();
	~VulkanPipeline();

public:
	void createPipelineCache();
	bool createPipeline(VulkanDrawable* drawableObject, VkPipeline* pipeline, VulkanShader* shaderObject, VkBool32 includeDepth, VkBool32 includeVi = true);
	void destroyPipelineCache();

private:
	VkPipelineCache pipelineCache = nullptr;
	VkPipelineLayout pipelineLayout = nullptr;
	VulkanApplication* appObject;
	VulkanDevice* deviceObject;
};

#endif /* !VULKAN_PIPELINE_H */
