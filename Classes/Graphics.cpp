#include "Graphics.hpp"

bool cGraphic::initInstance()
{
	if (gInstance != VK_NULL_HANDLE) 
		return false;

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vk_project_1";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "None";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	unsigned int extensionCount = 0;
	std::vector<VkExtensionProperties> extensionProperties;
	const char* extensionNames[] = { "VK_KHR_surface", "VK_KHR_win32_surface",  "VK_EXT_debug_report"};

	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	extensionProperties.resize(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());

	bool checkSurfaceExtension = false, checkWin32SurfaceExtension = false;

	for (unsigned int i = 0; i < extensionProperties.size(); i++) {
		if (strcmp(extensionProperties.at(i).extensionName, "VK_KHR_surface")) 
			checkSurfaceExtension = true;
		if (strcmp(extensionProperties.at(i).extensionName, "VK_KHR_win32_surface"))
			checkWin32SurfaceExtension = true;
	}
	assert(checkWin32SurfaceExtension || checkSurfaceExtension);

	createInfo.enabledExtensionCount = 3;
	createInfo.ppEnabledExtensionNames = extensionNames;

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &gInstance);
	if (result != VK_SUCCESS) assert(false);

	return true;
}

bool cGraphic::initSurface()
{
	if (gSurface != VK_NULL_HANDLE) return false;

	VkWin32SurfaceCreateInfoKHR sci;
	sci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	sci.hwnd = pWnd->getHWND();
	sci.hinstance = pWnd->getHInstance();
	sci.pNext = nullptr;
	sci.flags = 0;

	auto CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(gInstance, "vkCreateWin32SurfaceKHR");

	VkResult result = vkCreateWin32SurfaceKHR(gInstance, &sci, nullptr, &gSurface);

	if (result != VK_SUCCESS || !CreateWin32SurfaceKHR)
	{
		assert(1 && "Error");
		return false;
	}

	return true;
}

bool cGraphic::initPhysicalDevice()
{

	//get count Devices
	uint32_t countDevices = 0;
	vkEnumeratePhysicalDevices(gInstance ,&countDevices, nullptr);

	//get valid layers
	std::vector<VkPhysicalDevice> aDevices(countDevices);
	vkEnumeratePhysicalDevices(gInstance ,&countDevices, aDevices.data());

	if (countDevices == 0 || !isDeviceSuitable(&aDevices.at(0), &gSurface))
	{
		assert(1 && "Error");
		return false;
	}

	gPhysicalDevice = aDevices.at(0);

	findQueueFamily(&gPhysicalDevice, &gSurface);

	return true;
}

bool cGraphic::initLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamily(&gPhysicalDevice, &gSurface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

	float queuePriority = 1.0f;
	for (int queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo;
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	
	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	if (enableValidationLayers) {
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		deviceCreateInfo.enabledLayerCount = 0;
	}
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	VkResult result = vkCreateDevice(gPhysicalDevice, &deviceCreateInfo, nullptr, &gDevice);

	if (result != VK_SUCCESS)
		return false;

	vkGetDeviceQueue(gDevice, indices.graphicsFamily, 0, &gQueue);

	return true;
}

bool cGraphic::initSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(&gPhysicalDevice, &gSurface);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		imageCount = swapChainSupport.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = gSurface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	//createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indeces = findQueueFamily(&gPhysicalDevice, &gSurface);
	uint32_t queueFamilyIndeces[] = { (uint32_t)indeces.graphicsFamily, (uint32_t)indeces.presentFamily };

	if (indeces.graphicsFamily != indeces.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndeces;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(gDevice, &createInfo, nullptr, &gSwapChain);

	if (result != VK_SUCCESS)
	{
		return false;
	}

	return true;
}


bool cGraphic::initDeviceQueue()
{
	VkDeviceQueueCreateInfo queue_info = {};

	uint32_t queue_family_count = 0;

	vkGetPhysicalDeviceQueueFamilyProperties(gPhysicalDevice, &queue_family_count, NULL);
	std::vector<VkQueueFamilyProperties> prop(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(gPhysicalDevice, &queue_family_count, prop.data());

	vkGetDeviceQueue(gDevice, queue_family_count, 0, &gQueue);

	

	VkCommandPoolCreateInfo cpci = {};
	cpci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cpci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	for (UINT i = 0; i > prop.size(); i++)
		if (prop[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
			cpci.queueFamilyIndex = i;

	VkCommandPool commandPool;
	VkResult result = vkCreateCommandPool(gDevice, &cpci, nullptr, &commandPool);
	if (result != VK_SUCCESS)
		return false;

	VkCommandBufferAllocateInfo cbai = {};
	cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cbai.commandPool = commandPool;
	cbai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cbai.commandBufferCount = 1;

	result = vkAllocateCommandBuffers(gDevice, &cbai, &gCommandBuffer);

	if (result != VK_SUCCESS)
		return false;

	return true;
}

bool cGraphic::initFramebuffer()
{
	VkAttachmentDescription pass = {};
	pass.format = VK_FORMAT_B8G8R8A8_UNORM;
	pass.samples = VK_SAMPLE_COUNT_1_BIT;

	pass.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	pass.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	pass.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	pass.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	pass.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	pass.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference ar = {};
	ar.attachment = 0;
	ar.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &ar;
	subpass.pDepthStencilAttachment = nullptr;

	VkRenderPassCreateInfo rpci = {};
	rpci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	rpci.attachmentCount = 1;
	rpci.pAttachments = &pass;
	rpci.subpassCount = 1;
	rpci.pSubpasses = &subpass;

	VkResult result = vkCreateRenderPass(gDevice, &rpci, nullptr, &gRenderPass);

	if (result != VK_SUCCESS)
		return false;

	VkImageView frameBufferAttachments[1] = { 0 };

	VkFramebufferCreateInfo fbci = {};
	fbci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fbci.renderPass = gRenderPass;

	fbci.attachmentCount = 1;
	fbci.pAttachments = frameBufferAttachments;
	fbci.width = surfaceResolution.width;
	fbci.width = surfaceResolution.height;
	fbci.layers = 1;

	gFrameBuffer = new VkFramebuffer[2];
	for (uint32_t i = 0; i < 2; i++)
	{
		frameBufferAttachments[0] = gPresentImages[i];
		result = vkCreateFramebuffer(gDevice, &fbci, nullptr, &gFrameBuffer[i]);
		if (result != VK_SUCCESS)
			return false;
	}

	return true;
}

void cGraphic::presentStep()
{
	uint32_t nextImageIdx;
	VkResult result = vkAcquireNextImageKHR(gDevice, gSwapChain, UINT64_MAX, VK_NULL_HANDLE, VK_NULL_HANDLE, &nextImageIdx);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = nullptr;
	beginInfo.pNext = nullptr;
	static float aa = 0.0f;
	aa += 0.001f;
	if (aa > 1.0f) aa = 0;
	VkClearValue clearValue;
	clearValue.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	clearValue.depthStencil.depth = 1.0f;
	clearValue.depthStencil.stencil = 0.0f;
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = gRenderPass;
	renderPassBeginInfo.framebuffer = gFrameBuffer[nextImageIdx];
	VkRect2D rect = { { 0.0f, 0.0f } , surfaceResolution };
	renderPassBeginInfo.renderArea = rect;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearValue;
	renderPassBeginInfo.pNext = nullptr;
	

	result = vkBeginCommandBuffer(gCommandBuffer, &beginInfo);
		vkCmdBeginRenderPass(gCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		//draw some s**t:3
	
		vkCmdEndRenderPass(gCommandBuffer);
	vkEndCommandBuffer(gCommandBuffer);

	///

	VkFence renderFence;

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	vkCreateFence(gDevice, &fenceCreateInfo, nullptr, &renderFence);

	VkSubmitInfo si = {};
	si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	si.waitSemaphoreCount = 0;
	si.pWaitSemaphores = VK_NULL_HANDLE;
	si.pWaitDstStageMask = nullptr;
	si.commandBufferCount = 1;
	si.pCommandBuffers = &gCommandBuffer;
	si.signalSemaphoreCount = 0;
	si.pSignalSemaphores = VK_NULL_HANDLE;

	vkQueueSubmit(gQueue, 1, &si, renderFence);

	vkWaitForFences(gDevice, 1, &renderFence, VK_TRUE, UINT64_MAX);
	vkDestroyFence(gDevice, renderFence, nullptr);

	VkPresentInfoKHR pi = {};
	pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	pi.pNext = nullptr;
	pi.waitSemaphoreCount = 0;
	pi.pWaitSemaphores = VK_NULL_HANDLE;
	pi.swapchainCount = 1;
	pi.pSwapchains = &gSwapChain;
	pi.pImageIndices = &nextImageIdx;
	pi.pResults = nullptr;

	vkQueuePresentKHR(gQueue, &pi);
}

cGraphic::~cGraphic()
{
	vkDestroyInstance(gInstance, NULL);
}


QueueFamilyIndices cGraphic::findQueueFamily(VkPhysicalDevice *pDevice, VkSurfaceKHR *pSurface)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;

	vkGetPhysicalDeviceQueueFamilyProperties(*pDevice, &queueFamilyCount, NULL);
	std::vector<VkQueueFamilyProperties> prop(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(*pDevice, &queueFamilyCount, prop.data());


	int i = 0;


	for (const auto& queueFamily : prop)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(*pDevice, i, *pSurface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport)
		{
			indices.presentFamily = i;
		}
		if (indices.isComplete()) break;

		i++;
	}

	return indices;
}
SwapChainSupportDetails cGraphic::querySwapChainSupport(VkPhysicalDevice * pDevice, VkSurfaceKHR *pSurface)
{
	SwapChainSupportDetails details;

	VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*pDevice, gSurface, &details.capabilities);

	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(*pDevice, *pSurface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(*pDevice, *pSurface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(*pDevice, *pSurface, &presentModeCount, nullptr);
	
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(*pDevice, *pSurface, &presentModeCount, details.presentModes.data());
	}


	return details;
}
bool cGraphic::isDeviceSuitable(VkPhysicalDevice *pDevice, VkSurfaceKHR *pSurface)
{
	QueueFamilyIndices indeces = findQueueFamily(pDevice, pSurface);

	bool extensionsSupported = checkDeviceExtensionSupport(pDevice);
	bool swapChainSupportAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails details = querySwapChainSupport(pDevice, pSurface);
		swapChainSupportAdequate = !details.formats.empty() && !details.presentModes.empty();
	}

	return indeces.isComplete() && extensionsSupported && swapChainSupportAdequate;
}

bool cGraphic::checkDeviceExtensionSupport(VkPhysicalDevice * pDevice)
{
	//Показывает допустимые расширения и удаляет из нашего запроса расширения которые нашел. 
	uint32_t extensionCount;

	vkEnumerateDeviceExtensionProperties(*pDevice, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> avilableExtension(extensionCount);

	vkEnumerateDeviceExtensionProperties(*pDevice, nullptr, &extensionCount, avilableExtension.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : avilableExtension)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool cGraphic::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	for (const char* layerName : validationLayers) 
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) 
		{
			if (strcmp(layerName, layerProperties.layerName) == 0) 
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound) return false;
	}

	return true;
}

VkSurfaceFormatKHR cGraphic::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avilableFormats)
{
	if (avilableFormats.size() == 1 && avilableFormats.at(0).format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& avilableFormat : avilableFormats)
	{
		if (avilableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && avilableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return avilableFormat;
		}
	}
	return avilableFormats.at(0);
}
VkPresentModeKHR cGraphic::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D cGraphic::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = { 0,0 };
		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData) {

	std::cerr << "validation layer: " << msg << std::endl;

	return VK_FALSE;
}
VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}
void cGraphic::setupDebugCallback()
{
	if (!enableValidationLayers) return;
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;
	VkResult result = CreateDebugReportCallbackEXT(gInstance, &createInfo, nullptr, &callback);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug callback!");
	}
}

