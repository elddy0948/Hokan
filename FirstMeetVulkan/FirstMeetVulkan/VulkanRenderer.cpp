#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer(VulkanApplication* app, VulkanDevice* device) {
	assert(application != nullptr);
	assert(deviceObject != nullptr);

	//memset(&Depth, 0, sizeof(Depth));
	memset(connection, 0, sizeof(HINSTANCE));

	application = app;
	deviceObject = device;

	// swapchain init
}

VulkanRenderer::~VulkanRenderer() {
	// delete swapchain object
}

void VulkanRenderer::initialize() {
	createPresentationWindow(500, 500);
	// swap chain init
	createCommandPool();
	buildSwapChainAndDepthImage();
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

LRESULT VulkanRenderer::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	VulkanApplication* app = VulkanApplication::GetApp();
	switch (uMsg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return (DefWindowProc(hwnd, uMsg, wParam, lParam));
}

void VulkanRenderer::createPresentationWindow(const int& windowWidth, const int& windowHeight) {
#ifdef _WIN32
	width = windowWidth;
	height = windowHeight;

	assert(width > 0 || height > 0);

	WNDCLASSEX windowInfo;
	sprintf_s(name, "SwapChain presentation window");
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

	RECT wr = { 0, 0, width, height };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	window = CreateWindowEx(0, name, name, WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU, 100, 100, wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr, connection, nullptr);

	if (!window) {
		printf("[ ! ] : Can't create window in which to draw!\n");
		fflush(stdout);
		exit(1);
	}
	SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)&application);

#endif // _WIN32

}

void VulkanRenderer::destroyPresentationWindow() {
	DestroyWindow(window);
}

void VulkanRenderer::createCommandPool() {
	VulkanDevice* deviceObject = application->getDevice();
	VkResult result;
	VkCommandPoolCreateInfo commandPoolCreateInfo = {};

	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.pNext = nullptr;
	commandPoolCreateInfo.queueFamilyIndex = deviceObject->getGraphicsQueueFamilyIndex();
	commandPoolCreateInfo.flags = 0;

	result = vkCreateCommandPool(*deviceObject->getDevice(), &commandPoolCreateInfo, nullptr, &commandPool);
	if (result != VK_SUCCESS) { std::cout << "[ ! ] : Failed to create command pool!\n"; }
}

void VulkanRenderer::buildSwapChainAndDepthImage() {
	deviceObject->getDeviceQueue();
	swapChainObject->createSwapChain(commandDepthImage);
	createDepthImage();
}


