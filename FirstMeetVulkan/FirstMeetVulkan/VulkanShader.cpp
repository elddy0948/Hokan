#include "VulkanShader.h"
#include "VulkanApplication.h"
#include "VulkanDevice.h"

void VulkanShader::buildShaderModuleWithSPV(uint32_t* vertexShaderText, size_t vertexSPVSize, uint32_t* fragmentShaderText, size_t fragmentSPVSize) {
	VulkanDevice* deviceObject = VulkanApplication::GetApp()->getVulkanDevice();
	VkResult result;
	VkShaderModuleCreateInfo moduleCreateInfo = {};

	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].pNext = nullptr;
	shaderStages[0].pSpecializationInfo = nullptr;
	shaderStages[0].flags = 0;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].pName = "main";

	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = nullptr;
	moduleCreateInfo.flags = 0;
	moduleCreateInfo.codeSize = vertexSPVSize;
	moduleCreateInfo.pCode = vertexShaderText;

	result = vkCreateShaderModule(*deviceObject->getVkDevice(), &moduleCreateInfo, nullptr, &shaderStages[0].module);
	assert(result == VK_SUCCESS);

	std::vector<unsigned int> fragmentSPV;

	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].pNext = nullptr;
	shaderStages[1].pSpecializationInfo = nullptr;
	shaderStages[1].flags = 0;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].pName = "main";

	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = nullptr;
	moduleCreateInfo.codeSize = fragmentSPVSize;
	moduleCreateInfo.pCode = fragmentShaderText;
	
	result = vkCreateShaderModule(*deviceObject->getVkDevice(), &moduleCreateInfo, nullptr, &shaderStages[1].module);
	assert(result == VK_SUCCESS);
}

void VulkanShader::destroyShaders() {
	VulkanDevice* deviceObject = VulkanApplication::GetApp()->getVulkanDevice();
	vkDestroyShaderModule(*deviceObject->getVkDevice(), shaderStages[0].module, nullptr);
	vkDestroyShaderModule(*deviceObject->getVkDevice(), shaderStages[1].module, nullptr);
}


#ifdef AUTO_COMPILE_GLSL_TO_SPV
bool VulkanShader::GLSLtoSPV(const VkShaderStageFlagBits shaderType, const char* pShader, std::vector<unsigned int>& spirv) {
	glslang::TProgram* program = new glslang::TProgram;
	const char* shaderStrings[1];
	TBuiltInResource Resources;

	initializeResources(Resources);
	EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
	EShLanguage stage = getLanguage(shaderType);

	glslang::TShader* shader = new glslang::TShader(stage);

	shaderStrings[0] = pShader;
	shader->setStrings(shaderStrings, 1);

	if (!shader->parse(&Resources, 100, false, messages)) {
		puts(shader->getInfoLog());
		puts(shader->getInfoDebugLog());
		return false;
	}

	program->addShader(shader);

	if (!program->link(messages)) {
		puts(shader->getInfoLog());
		puts(shader->getInfoDebugLog());
		return false;
	}

	glslang::GlslangToSpv(*program->getIntermediate(stage), spirv);

	delete program;
	delete shader;

	return true;
}

void VulkanShader::buildShader(const char* vertexShaderText, const char* fragmentShaderText) {
	VulkanDevice* deviceObject = VulkanApplication::GetApp()->getVulkanDevice();
	VkResult result;
	bool returnValue;
	VkShaderModuleCreateInfo moduleCreateInfo = {};

	std::vector<unsigned int> vertexSPV;
	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].pNext = nullptr;
	shaderStages[0].pSpecializationInfo = nullptr;
	shaderStages[0].flags = 0;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].pName = "main";

	glslang::InitializeProcess();

	returnValue = GLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, vertexShaderText, vertexSPV);
	assert(returnValue);

	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = nullptr;
	moduleCreateInfo.flags = 0;
	moduleCreateInfo.codeSize = vertexSPV.size() * sizeof(unsigned int);
	moduleCreateInfo.pCode = vertexSPV.data();

	result = vkCreateShaderModule(*deviceObject->getVkDevice(), &moduleCreateInfo, nullptr, &shaderStages[0].module);
	assert(result == VK_SUCCESS);

	std::vector<unsigned int> fragmentSPV;
	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].pNext = nullptr;
	shaderStages[1].pSpecializationInfo = nullptr;
	shaderStages[1].flags = 0;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].pName = "main";

	returnValue = GLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShaderText, fragmentSPV);
	assert(returnValue);

	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = nullptr;
	moduleCreateInfo.flags = 0;
	moduleCreateInfo.codeSize = fragmentSPV.size() * sizeof(unsigned int);
	moduleCreateInfo.pCode = fragmentSPV.data();

	result = vkCreateShaderModule(*deviceObject->getVkDevice(), &moduleCreateInfo, nullptr, &shaderStages[1].module);
	assert(result == VK_SUCCESS);

	glslang::FinalizeProcess();


}

EShLanguage VulkanShader::getLanguage(const VkShaderStageFlagBits shaderType) {
	switch (shaderType) {
	case VK_SHADER_STAGE_VERTEX_BIT:
		return EShLangVertex;
	case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
		return EShLangTessControl;
	case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
		return EShLangTessEvaluation;
	case VK_SHADER_STAGE_GEOMETRY_BIT:
		return EShLangGeometry;
	case VK_SHADER_STAGE_FRAGMENT_BIT:
		return EShLangFragment;
	case VK_SHADER_STAGE_COMPUTE_BIT:
		return EShLangCompute;
	default:
		std::cout << "Unknown shader type specified : " << shaderType << ". Exiting!\n";
		exit(1);
	}
}

void VulkanShader::initializeResources(TBuiltInResource& Resources) {
	Resources.maxLights = 32;
	Resources.maxClipPlanes = 6;
	Resources.maxTextureUnits = 32;
	Resources.maxTextureCoords = 32;
	Resources.maxVertexAttribs = 64;
	Resources.maxVertexUniformComponents = 4096;
	Resources.maxVaryingFloats = 64;
	Resources.maxVertexTextureImageUnits = 32;
	Resources.maxCombinedTextureImageUnits = 80;
	Resources.maxTextureImageUnits = 32;
	Resources.maxFragmentUniformComponents = 4096;
	Resources.maxDrawBuffers = 32;
	Resources.maxVertexUniformVectors = 128;
	Resources.maxVaryingVectors = 8;
	Resources.maxFragmentUniformVectors = 16;
	Resources.maxVertexOutputVectors = 16;
	Resources.maxFragmentInputVectors = 15;
	Resources.minProgramTexelOffset = -8;
	Resources.maxProgramTexelOffset = 7;
	Resources.maxClipDistances = 8;
	Resources.maxComputeWorkGroupCountX = 65535;
	Resources.maxComputeWorkGroupCountY = 65535;
	Resources.maxComputeWorkGroupCountZ = 65535;
	Resources.maxComputeWorkGroupSizeX = 1024;
	Resources.maxComputeWorkGroupSizeY = 1024;
	Resources.maxComputeWorkGroupSizeZ = 64;
	Resources.maxComputeUniformComponents = 1024;
	Resources.maxComputeTextureImageUnits = 16;
	Resources.maxComputeImageUniforms = 8;
	Resources.maxComputeAtomicCounters = 8;
	Resources.maxComputeAtomicCounterBuffers = 1;
	Resources.maxVaryingComponents = 60;
	Resources.maxVertexOutputComponents = 64;
	Resources.maxGeometryInputComponents = 64;
	Resources.maxGeometryOutputComponents = 128;
	Resources.maxFragmentInputComponents = 128;
	Resources.maxImageUnits = 8;
	Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
	Resources.maxCombinedShaderOutputResources = 8;
	Resources.maxImageSamples = 0;
	Resources.maxVertexImageUniforms = 0;
	Resources.maxTessControlImageUniforms = 0;
	Resources.maxTessEvaluationImageUniforms = 0;
	Resources.maxGeometryImageUniforms = 0;
	Resources.maxFragmentImageUniforms = 8;
	Resources.maxCombinedImageUniforms = 8;
	Resources.maxGeometryTextureImageUnits = 16;
	Resources.maxGeometryOutputVertices = 256;
	Resources.maxGeometryTotalOutputComponents = 1024;
	Resources.maxGeometryUniformComponents = 1024;
	Resources.maxGeometryVaryingComponents = 64;
	Resources.maxTessControlInputComponents = 128;
	Resources.maxTessControlOutputComponents = 128;
	Resources.maxTessControlTextureImageUnits = 16;
	Resources.maxTessControlUniformComponents = 1024;
	Resources.maxTessControlTotalOutputComponents = 4096;
	Resources.maxTessEvaluationInputComponents = 128;
	Resources.maxTessEvaluationOutputComponents = 128;
	Resources.maxTessEvaluationTextureImageUnits = 16;
	Resources.maxTessEvaluationUniformComponents = 1024;
	Resources.maxTessPatchComponents = 120;
	Resources.maxPatchVertices = 32;
	Resources.maxTessGenLevel = 64;
	Resources.maxViewports = 16;
	Resources.maxVertexAtomicCounters = 0;
	Resources.maxTessControlAtomicCounters = 0;
	Resources.maxTessEvaluationAtomicCounters = 0;
	Resources.maxGeometryAtomicCounters = 0;
	Resources.maxFragmentAtomicCounters = 8;
	Resources.maxCombinedAtomicCounters = 8;
	Resources.maxAtomicCounterBindings = 1;
	Resources.maxVertexAtomicCounterBuffers = 0;
	Resources.maxTessControlAtomicCounterBuffers = 0;
	Resources.maxTessEvaluationAtomicCounterBuffers = 0;
	Resources.maxGeometryAtomicCounterBuffers = 0;
	Resources.maxFragmentAtomicCounterBuffers = 1;
	Resources.maxCombinedAtomicCounterBuffers = 1;
	Resources.maxAtomicCounterBufferSize = 16384;
	Resources.maxTransformFeedbackBuffers = 4;
	Resources.maxTransformFeedbackInterleavedComponents = 64;
	Resources.maxCullDistances = 8;
	Resources.maxCombinedClipAndCullDistances = 8;
	Resources.maxSamples = 4;
	Resources.limits.nonInductiveForLoops = 1;
	Resources.limits.whileLoops = 1;
	Resources.limits.doWhileLoops = 1;
	Resources.limits.generalUniformIndexing = 1;
	Resources.limits.generalAttributeMatrixVectorIndexing = 1;
	Resources.limits.generalVaryingIndexing = 1;
	Resources.limits.generalSamplerIndexing = 1;
	Resources.limits.generalVariableIndexing = 1;
	Resources.limits.generalConstantMatrixVectorIndexing = 1;
}

#endif

