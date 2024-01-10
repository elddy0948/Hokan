#include "VulkanPipeline.h"

#include "VulkanApplication.h"
#include "VulkanShader.h"
#include "VulkanRenderer.h"
#include "VulkanDevice.h"

VulkanPipeline::VulkanPipeline() {
	appObject = VulkanApplication::GetApp();
	deviceObject = appObject->getVulkanDevice();
}

VulkanPipeline::~VulkanPipeline() {}

void VulkanPipeline::createPipelineCache() {
	VkResult result;
	VkPipelineCacheCreateInfo pipelineCacheInfo = {};

	pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	pipelineCacheInfo.pNext = nullptr;
	pipelineCacheInfo.initialDataSize = 0;
	pipelineCacheInfo.pInitialData = nullptr;
	pipelineCacheInfo.flags = 0;

	result = vkCreatePipelineCache(*deviceObject->getVkDevice(), &pipelineCacheInfo, nullptr, &pipelineCache);

	assert(result == VK_SUCCESS);
}

bool VulkanPipeline::createPipeline(VulkanDrawable* drawableObject, VkPipeline* pipeline, VulkanShader* shaderObject, VkBool32 includeDepth, VkBool32 includeVi) {
	VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
	VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = {};

	dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.pNext = nullptr;
	dynamicStateInfo.pDynamicStates = nullptr;
	dynamicStateInfo.dynamicStateCount = 0;

	vertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStateInfo.pNext = nullptr;
	vertexInputStateInfo.flags = 0;

	if (includeVi) {
		vertexInputStateInfo.vertexBindingDescriptionCount = sizeof(drawableObject->vertexInputBind) / sizeof(VkVertexInputBindingDescription);
		vertexInputStateInfo.pVertexBindingDescriptions = &drawableObject->vertexInputBind;
		vertexInputStateInfo.vertexAttributeDescriptionCount = sizeof(drawableObject->vertexInputAttribute) / sizeof(VkVertexInputAttributeDescription);
		vertexInputStateInfo.pVertexAttributeDescriptions = drawableObject->vertexInputAttribute;
	}

	/* Input assembly */
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.pNext = nullptr;
	inputAssemblyInfo.flags = 0;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	/* Rasterization */
	VkPipelineRasterizationStateCreateInfo rasterizationStateInfo = {};
	rasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationStateInfo.pNext = nullptr;
	rasterizationStateInfo.flags = 0;
	rasterizationStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationStateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationStateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizationStateInfo.depthClampEnable = includeDepth;
	rasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationStateInfo.depthBiasEnable = VK_FALSE;	
	rasterizationStateInfo.depthBiasConstantFactor = 0;
	rasterizationStateInfo.depthBiasClamp = 0;
	rasterizationStateInfo.depthBiasSlopeFactor = 0;
	rasterizationStateInfo.lineWidth = 1.0f;

	/* Color blend */
	VkPipelineColorBlendAttachmentState colorBlendAttachmentState[1] = {};
	colorBlendAttachmentState[0].colorWriteMask = 0xf;
	colorBlendAttachmentState[0].blendEnable = VK_FALSE;
	colorBlendAttachmentState[0].alphaBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState[0].colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

	VkPipelineColorBlendStateCreateInfo colorBlendStateInfo = {};
	colorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendStateInfo.flags = 0;
	colorBlendStateInfo.pNext = nullptr;
	colorBlendStateInfo.attachmentCount = 1;
	colorBlendStateInfo.pAttachments = colorBlendAttachmentState;
	colorBlendStateInfo.logicOpEnable = VK_FALSE;
	colorBlendStateInfo.logicOp = VK_LOGIC_OP_NO_OP;
	colorBlendStateInfo.blendConstants[0] = 1.0f;
	colorBlendStateInfo.blendConstants[1] = 1.0f;
	colorBlendStateInfo.blendConstants[2] = 1.0f;
	colorBlendStateInfo.blendConstants[3] = 1.0f;

	/* Viewport */
	VkPipelineViewportStateCreateInfo viewportStateInfo = {};
	viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateInfo.pNext = nullptr;
	viewportStateInfo.flags = 0;
	viewportStateInfo.viewportCount = NUMBER_OF_VIEWPORTS;
	viewportStateInfo.scissorCount = NUMBER_OF_SCISSORS;
	viewportStateInfo.pScissors = nullptr;
	viewportStateInfo.pViewports = nullptr;

	//TODO: - Dynamic state enables..

	/* Depth stencil */
	VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo = {};
	depthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilStateInfo.pNext = nullptr;
	depthStencilStateInfo.flags = 0;
	depthStencilStateInfo.depthTestEnable = includeDepth;
	depthStencilStateInfo.depthWriteEnable = includeDepth;
	depthStencilStateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencilStateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilStateInfo.stencilTestEnable = VK_FALSE;

	depthStencilStateInfo.back.failOp = VK_STENCIL_OP_KEEP;
	depthStencilStateInfo.back.passOp = VK_STENCIL_OP_KEEP;
	depthStencilStateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
	depthStencilStateInfo.back.compareMask = 0;
	depthStencilStateInfo.back.reference = 0;
	depthStencilStateInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
	depthStencilStateInfo.back.writeMask = 0;

	depthStencilStateInfo.minDepthBounds = 0;
	depthStencilStateInfo.maxDepthBounds = 0;
	depthStencilStateInfo.stencilTestEnable = VK_FALSE;
	depthStencilStateInfo.front = depthStencilStateInfo.back;

	/* Multisample */
	VkPipelineMultisampleStateCreateInfo multisampleStateInfo = {};
	multisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleStateInfo.pNext = nullptr;
	multisampleStateInfo.flags = 0;
	multisampleStateInfo.pSampleMask = nullptr;
	multisampleStateInfo.rasterizationSamples = NUM_SAMPLES;
	multisampleStateInfo.sampleShadingEnable = VK_FALSE;
	multisampleStateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleStateInfo.alphaToOneEnable = VK_FALSE;
	multisampleStateInfo.minSampleShading = 0.0;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	VkResult result;
	result = vkCreatePipelineLayout(*deviceObject->getVkDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout);

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = nullptr;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.basePipelineHandle = 0;
	pipelineInfo.basePipelineIndex = 0;
	pipelineInfo.flags = 0;
	pipelineInfo.pVertexInputState = &vertexInputStateInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pRasterizationState = &rasterizationStateInfo;
	pipelineInfo.pColorBlendState = &colorBlendStateInfo;
	pipelineInfo.pTessellationState = nullptr;
	pipelineInfo.pMultisampleState = &multisampleStateInfo;
	pipelineInfo.pDynamicState = &dynamicStateInfo;
	pipelineInfo.pViewportState = &viewportStateInfo;
	pipelineInfo.pDepthStencilState = &depthStencilStateInfo;
	pipelineInfo.pStages = shaderObject->shaderStages;
	pipelineInfo.stageCount = 2;
	pipelineInfo.renderPass = appObject->getRenderer()->renderPass;
	pipelineInfo.subpass = 0;

	if (vkCreateGraphicsPipelines(*deviceObject->getVkDevice(), pipelineCache, 1, &pipelineInfo, nullptr, pipeline) == VK_SUCCESS)
		return true;
	else
		return false;
}

void VulkanPipeline::destroyPipelineCache() {
	vkDestroyPipelineCache(*deviceObject->getVkDevice(), pipelineCache, nullptr);
	vkDestroyPipelineLayout(*deviceObject->getVkDevice(), pipelineLayout, nullptr);
}
