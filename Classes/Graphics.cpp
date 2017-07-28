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
	const char* extensionNames[] = { "VK_KHR_surface", "VK_KHR_win32_surface" };

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

	createInfo.enabledExtensionCount = 2;
	createInfo.ppEnabledExtensionNames = extensionNames;

	createInfo.enabledLayerCount = 0;

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


	VkResult result = vkCreateWin32SurfaceKHR(gInstance, &sci, nullptr, &gSurface);

	if (result != VK_SUCCESS)
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

	if (countDevices == 0)
	{
		assert(1 && "Error");
		return false;
	}
	gPhysicalDevice = aDevices.at(0);

	//VkPhysicalDeviceProperties physDevProp;
	//vkGetPhysicalDeviceProperties(aDevices.at(0), &physDevProp);

	return true;
}

bool cGraphic::initLogicalDevice()
{
	VkResult result;
	VkPhysicalDeviceMemoryProperties pdmp;
	vkGetPhysicalDeviceMemoryProperties(gPhysicalDevice, &pdmp);


	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(gPhysicalDevice, &queue_family_count, NULL);
	std::vector<VkQueueFamilyProperties> prop(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(gPhysicalDevice, &queue_family_count, prop.data());

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = 0;
	queueCreateInfo.queueCount = 1;
	
	float queuePriority[] = { 0.0f };
	queueCreateInfo.pQueuePriorities = queuePriority;

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;


	std::vector<const char *> layerNames;

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};
	deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();

	deviceCreateInfo.enabledExtensionCount = 0;
	deviceCreateInfo.ppEnabledExtensionNames = nullptr;

	VkPhysicalDeviceFeatures pdf = {};
	pdf.shaderClipDistance = VK_TRUE;
	deviceCreateInfo.pEnabledFeatures = &pdf;
	

	result = vkCreateDevice(gPhysicalDevice, &deviceCreateInfo, nullptr, &gDevice);

	if (result != VK_SUCCESS)
		return false;

	return true;
}

bool cGraphic::initSwapChain()
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities = {};

	VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gPhysicalDevice, gSurface, &surfaceCapabilities);
	surfaceResolution = surfaceCapabilities.currentExtent;

	VkSwapchainCreateInfoKHR ssci = {};
	ssci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	ssci.surface = gSurface;
	ssci.minImageCount = 2;
	ssci.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
	ssci.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	ssci.imageExtent = surfaceResolution;
	ssci.imageArrayLayers = 1;
	ssci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	ssci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	ssci.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	ssci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	ssci.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	ssci.clipped = true;
	ssci.oldSwapchain = VK_NULL_HANDLE;

	result = vkCreateSwapchainKHR(gDevice, &ssci, nullptr, &gSwapChain);
	if (result != VK_SUCCESS)
		return false;

	gPresentImages.resize(ssci.minImageCount);
	result = vkGetSwapchainImagesKHR(gDevice, gSwapChain, &ssci.minImageCount, gPresentImages.data());
	if (result != VK_SUCCESS)
		return false;
	gImageViews.resize(ssci.minImageCount);
	for (int i = 0; i < ssci.minImageCount; i++)
	{
		VkImageViewCreateInfo ivci = {};
		ivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ivci.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ivci.format = VK_FORMAT_B8G8R8A8_UNORM;
		ivci.components.r = VK_COMPONENT_SWIZZLE_R;
		ivci.components.g = VK_COMPONENT_SWIZZLE_G;
		ivci.components.b = VK_COMPONENT_SWIZZLE_B;
		ivci.components.a = VK_COMPONENT_SWIZZLE_A;
		ivci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ivci.subresourceRange.baseMipLevel = 0;
		ivci.subresourceRange.levelCount = 1;
		ivci.subresourceRange.baseArrayLayer = 0;
		ivci.subresourceRange.layerCount = 1;
		ivci.image = gPresentImages.at(i);

		result = vkCreateImageView(gDevice, &ivci, nullptr, &gImageViews.at(i));
		if (result != VK_SUCCESS)
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

	vkGetDeviceQueue(gDevice, queue_family_count, 0, &gPresentQueue);

	

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

	vkQueueSubmit(gPresentQueue, 1, &si, renderFence);

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

	vkQueuePresentKHR(gPresentQueue, &pi);
}

cGraphic::~cGraphic()
{
	vkDestroyInstance(gInstance, NULL);
}
