#pragma once

#ifndef VULKAN_SHADER_H
#define VULKAN_SHADER_H

#include "Headers.h"

class VulkanShader {
public:
	VulkanShader() {}
	~VulkanShader() {}

	void buildShaderModuleWithSPV(uint32_t* vertexShaderText, size_t vertexSPVSize, uint32_t* fragmentShaderText, size_t fragmentSPVSize);
	void destroyShaders();

#ifdef AUTO_COMPILE_GLSL_TO_SPV
	bool GLSLtoSPV(const VkShaderStageFlagBits shaderType, const char* pShader, std::vector<unsigned int>& spirv);
	void buildShader(const char* vertexShaderText, const char* fragmentShaderText);
	EShLanguage getLanguage(const VkShaderStageFlagBits shaderType);
	void initializeResources(TBuiltInResource& Resources);
#endif

	VkPipelineShaderStageCreateInfo shaderStages[2] = {};
};

#endif /* !VULKAN_SHADER_H */
