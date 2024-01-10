#include "VulkanRenderer.h"
#include "VulkanApplication.h"
#include "CommandBufferManager.h"
#include "Helper.h"
#include "MeshData.h"

VulkanRenderer::VulkanRenderer(VulkanDevice* device) {
	memset(&Depth, 0, sizeof(Depth));
	memset(&connection, 0, sizeof(HINSTANCE));

	deviceObject = device;
	swapChainObject = new VulkanSwapChain(this);

	VulkanDrawable* drawableObject = new VulkanDrawable(this);
	drawableList.push_back(drawableObject);

	//delete drawableObject;
}

VulkanRenderer::~VulkanRenderer() {
	delete swapChainObject;
	swapChainObject = nullptr;
}

void VulkanRenderer::initialize(const int width, const int height) {
	const bool includeDepth = true;
	this->width = width;
	this->height = height;

	createPresentationWindow();
	swapChainObject->initializeSwapChain();
	createCommandPool();
	buildSwapChainAndDepthImage();

	createRenderPass(includeDepth);
	createFrameBuffer(includeDepth);

	createShaders();
	createPipelineStateManagement();
}

void VulkanRenderer::prepare() {
	for (const auto& drawableObject : drawableList) {
		drawableObject->prepare();
	}
}

bool VulkanRenderer::render() {
	MSG msg;
	PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
	if (msg.message == WM_QUIT) { return false; }
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	RedrawWindow(window, nullptr, nullptr, RDW_INTERNALPAINT);
	return true;
}

LRESULT VulkanRenderer::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
 	VulkanApplication* app = VulkanApplication::GetApp();
	switch (uMsg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		for (const auto& drawableObject : app->getRenderer()->drawableList) {
			drawableObject->render();
		}
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
		}
		break;
	default:
		break;
	}
	return (DefWindowProc(hwnd, uMsg, wParam, lParam));
}

void VulkanRenderer::createPresentationWindow() {
#ifdef _WIN32
	assert(width > 0 || height > 0);

	WNDCLASSEX windowInfo;
	sprintf_s(name, "Hello Triangle");
	memset(&windowInfo, 0, sizeof(WNDCLASSEX));

	windowInfo.cbSize = sizeof(WNDCLASSEX);
	windowInfo.style = CS_HREDRAW | CS_VREDRAW;
	windowInfo.lpfnWndProc = WndProc;
	windowInfo.cbClsExtra = 0;
	windowInfo.cbWndExtra = 0;
	windowInfo.hInstance = connection;
	windowInfo.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	windowInfo.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowInfo.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowInfo.lpszMenuName = nullptr;
	windowInfo.lpszClassName = name;
	windowInfo.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);

	if (!RegisterClassEx(&windowInfo)) {
		printf("[ ! ] : Unexpected error tryying to start the application! \n");
		fflush(stdout);
		exit(1);
	}

	RECT wr = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	window = CreateWindowEx(0, name, name, WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr, connection, nullptr);

	if (!window) {
		printf("[ ! ] : Can't create window in which to draw!\n");
		fflush(stdout);
		exit(1);
	}

	SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)VulkanApplication::GetApp());

#endif // _WIN32
}

void VulkanRenderer::setImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask, const VkCommandBuffer& commandBuffer) {
	assert(commandBuffer != VK_NULL_HANDLE);
	assert(deviceObject->getQueue() != VK_NULL_HANDLE);

	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.pNext = nullptr;
	imageMemoryBarrier.srcAccessMask = srcAccessMask;
	imageMemoryBarrier.dstAccessMask = 0;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.layerCount = 1;

	if (oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	switch (newImageLayout) {
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
	}

	VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags dstStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	vkCmdPipelineBarrier(commandBuffer, srcStages, dstStages, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

void VulkanRenderer::createCommandPool() {
	VkResult result;
	VkCommandPoolCreateInfo commandPoolCreateInfo = {};

	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.pNext = nullptr;
	commandPoolCreateInfo.queueFamilyIndex = deviceObject->getGraphicsQueueFamilyIndex();
	commandPoolCreateInfo.flags = 0;

	result = vkCreateCommandPool(*deviceObject->getVkDevice(), &commandPoolCreateInfo, nullptr, &commandPool);
	if (result != VK_SUCCESS) { std::cout << "[ ! ] : Failed to create command pool!\n"; }
}

void VulkanRenderer::buildSwapChainAndDepthImage() {
	deviceObject->getDeviceQueue();
	swapChainObject->createSwapChain(commandDepthImage);
	createDepthImage();
}

void VulkanRenderer::createDepthImage() {
	VkResult result;
	bool pass;

	VkImageCreateInfo imageInfo = {};

	if (Depth.format == VK_FORMAT_UNDEFINED) {
		Depth.format = VK_FORMAT_D16_UNORM;
	}

	const VkFormat depthFormat = Depth.format;

	VkFormatProperties properties;
	vkGetPhysicalDeviceFormatProperties(*deviceObject->getPhysicalDevice(), depthFormat, &properties);
	if (properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	}
	else if (properties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
		imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
	}
	else {
		std::cout << "Unsupported Depth format, try other Depth formats.\n";
		exit(-1);
	}

	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext = nullptr;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = depthFormat;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = NUM_SAMPLES;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageInfo.flags = 0;

	result = vkCreateImage(*deviceObject->getVkDevice(), &imageInfo, nullptr, &Depth.image);
	assert(result == VK_SUCCESS);

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(*deviceObject->getVkDevice(), Depth.image, &memoryRequirements);

	VkMemoryAllocateInfo memoryInfo = {};
	memoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryInfo.pNext = nullptr;
	memoryInfo.allocationSize = 0;
	memoryInfo.memoryTypeIndex = 0;
	memoryInfo.allocationSize = memoryRequirements.size;

	pass = deviceObject->memoryTypeFromProperties(memoryRequirements.memoryTypeBits, 0, &memoryInfo.memoryTypeIndex);
	assert(pass);

	result = vkAllocateMemory(*deviceObject->getVkDevice(), &memoryInfo, nullptr, &Depth.deviceMemory);
	assert(result == VK_SUCCESS);

	result = vkBindImageMemory(*deviceObject->getVkDevice(), Depth.image, Depth.deviceMemory, 0);
	assert(result == VK_SUCCESS);

	VkImageViewCreateInfo imageViewInfo = {};
	imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewInfo.pNext = nullptr;
	imageViewInfo.image = VK_NULL_HANDLE;
	imageViewInfo.format = depthFormat;
	imageViewInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY };
	imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	imageViewInfo.subresourceRange.baseMipLevel = 0;
	imageViewInfo.subresourceRange.levelCount = 1;
	imageViewInfo.subresourceRange.baseArrayLayer = 0;
	imageViewInfo.subresourceRange.layerCount = 1;
	imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewInfo.flags = 0;

	if (depthFormat == VK_FORMAT_D16_UNORM_S8_UINT || depthFormat == VK_FORMAT_D24_UNORM_S8_UINT || depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT) {
		imageViewInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	CommandBufferManager::allocCommandBuffer(deviceObject->getVkDevice(), commandPool, &commandDepthImage);
	CommandBufferManager::beginCommandBuffer(commandDepthImage);
	{
		setImageLayout(Depth.image, imageViewInfo.subresourceRange.aspectMask, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, (VkAccessFlagBits)0, commandDepthImage);
	}
	CommandBufferManager::endCommandBuffer(commandDepthImage);
	CommandBufferManager::submitCommandBuffer(*deviceObject->getQueue(), &commandDepthImage);

	imageViewInfo.image = Depth.image;
	result = vkCreateImageView(*deviceObject->getVkDevice(), &imageViewInfo, nullptr, &Depth.imageView);
	assert(result == VK_SUCCESS);
}

void VulkanRenderer::createRenderPass(bool includeDepth, bool clear) {
	VkResult result;
	VkAttachmentDescription attachments[2];
	VkAttachmentReference colorReference = {};
	VkAttachmentReference depthReference = {};
	VkSubpassDescription subpass = {};
	VkRenderPassCreateInfo renderPassInfo = {};

	attachments[0].format = swapChainObject->swapChainPublicVariables.format;
	attachments[0].samples = NUM_SAMPLES;
	attachments[0].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	attachments[0].flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;

	if (includeDepth) {
		attachments[1].format = Depth.format;
		attachments[1].samples = NUM_SAMPLES;
		attachments[1].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachments[1].flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
	}

	colorReference.attachment = 0;
	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	depthReference.attachment = 1;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.flags = 0;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = nullptr;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorReference;
	subpass.pResolveAttachments = nullptr;
	subpass.pDepthStencilAttachment = includeDepth ? &depthReference : nullptr;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;

	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = includeDepth ? 2 : 1;
	renderPassInfo.pAttachments = attachments;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 0;
	renderPassInfo.pDependencies = nullptr;

	result = vkCreateRenderPass(*deviceObject->getVkDevice(), &renderPassInfo, nullptr, &renderPass);
	assert(result == VK_SUCCESS);
}

void VulkanRenderer::createFrameBuffer(bool includeDepth) {
	VkResult result;
	VkImageView attachments[2];
	VkFramebufferCreateInfo framebufferInfo = {};
	uint32_t i;

	attachments[1] = Depth.imageView;

	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.pNext = nullptr;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = includeDepth ? 2 : 1;
	framebufferInfo.pAttachments = attachments;
	framebufferInfo.width = width;
	framebufferInfo.height = height;
	framebufferInfo.layers = 1;

	framebuffers.clear();
	framebuffers.resize(swapChainObject->swapChainPublicVariables.swapChainImageCount);

	for (i = 0; i < swapChainObject->swapChainPublicVariables.swapChainImageCount; ++i) {
		attachments[0] = swapChainObject->swapChainPublicVariables.colorBuffer[i].view;
		result = vkCreateFramebuffer(*deviceObject->getVkDevice(), &framebufferInfo, nullptr, &framebuffers.at(i));
		assert(result == VK_SUCCESS);
	}
}

void VulkanRenderer::createShaders() {
	void* vertexShaderCode;
	void* fragmentShaderCode;
	size_t vertexShaderSize;
	size_t fragmentShaderSize;

#ifdef AUTO_COMPILE_GLSL_TO_SPV
	vertexShaderCode = readFile("./../Draw.vert", &vertexShaderSize);
	fragmentShaderCode = readFile("./../Draw.frag", &fragmentShaderSize);

	shaderObject.buildShader((const char*)vertexShaderCode, (const char*)fragmentShaderCode);
#else
	vertexShaderCode = readFile("./Draw-vert.spv", &vertexShaderSize);
	fragmentShaderCode = readFile("./Draw-frag.spv", &fragmentShaderSize);

	shaderObject.buildShaderModuleWithSPV(static_cast<uint32_t*>(vertexShaderCode), vertexShaderSize, static_cast<uint32_t*>(fragmentShaderCode), fragmentShaderSize);
#endif // AUTO_COMPILE_GLSL_TO_SPV

}

void VulkanRenderer::createPipelineStateManagement() {
	pipelineObject.createPipelineCache();
	const bool depthPresent = true;
	
	for (const auto& drawable : drawableList) {
		VkPipeline* pipeline = (VkPipeline*)malloc(sizeof(VkPipeline));
		if (pipelineObject.createPipeline(drawable, pipeline, &shaderObject, depthPresent)) {
			pipelineList.push_back(pipeline);
			drawable->setPipeline(pipeline);
		}
		else {
			free(pipeline);
			pipeline = nullptr;
		}
	}
}

void VulkanRenderer::deinitialize() {
	VkCommandBuffer buffers[] = { commandDepthImage };

	/* Pipeline */
	for (const auto& pipeline : pipelineList) {
		vkDestroyPipeline(*deviceObject->getVkDevice(), *pipeline, nullptr);
		free(pipeline);
	}
	pipelineList.clear();

	pipelineObject.destroyPipelineCache();

	/* Frame buffer */
	for (uint32_t i = 0; i < swapChainObject->swapChainPublicVariables.swapChainImageCount; ++i) {
		vkDestroyFramebuffer(*deviceObject->getVkDevice(), framebuffers.at(i), nullptr);
	}
	framebuffers.clear();

	/* Reder pass */
	vkDestroyRenderPass(*deviceObject->getVkDevice(), renderPass, nullptr);

	/* Depth buffer */
	vkDestroyImageView(*deviceObject->getVkDevice(), Depth.imageView, nullptr);
	vkDestroyImage(*deviceObject->getVkDevice(), Depth.image, nullptr);
	vkFreeMemory(*deviceObject->getVkDevice(), Depth.deviceMemory, nullptr);

	/* Swapchain */
	swapChainObject->destroySwapChain();

	/* Command buffer */
	vkFreeCommandBuffers(*deviceObject->getVkDevice(), commandPool, sizeof(buffers) / sizeof(VkCommandBuffer), buffers);

	/* Command pool */
	vkDestroyCommandPool(*deviceObject->getVkDevice(), commandPool, nullptr);
	
	/* Presentation window */
	DestroyWindow(window);
}