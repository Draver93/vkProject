#ifndef GRAPHICS_H
#define GRAPHICS_H

#define NOMINMAX

#include <cassert> 
#include <cstdlib>
#include <vector>
#include <set>
#include <algorithm>

#define VK_USE_PLATFORM_WIN32_KHR

#include "vulkan/vulkan.h"
#include <vulkan/vk_sdk_platform.h>

#include "Window.hpp"


#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices
{
	int graphicsFamily = -1;
	int presentFamily = -1;
	bool isComplete()
	{
		return graphicsFamily >= 0 && presentFamily >= 0;
	}
};
struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class cGraphic
{
private:
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	const std::vector<const char*> validationLayers = {	"VK_LAYER_LUNARG_standard_validation" };

	cWindow *pWnd;
	VkDebugReportCallbackEXT callback;

	VkInstance gInstance;
	VkSurfaceKHR gSurface;
	VkPhysicalDevice gPhysicalDevice;
	VkDevice gDevice;
	VkSwapchainKHR gSwapChain;
	VkExtent2D surfaceResolution;
	VkQueue gQueue;
	VkCommandBuffer gCommandBuffer;

	VkFramebuffer *gFrameBuffer;
	VkRenderPass gRenderPass;

	std::vector<VkImage> gPresentImages; //Отображаемые кадры
	std::vector<VkImageView> gImageViews; //указатели через которые можно управлять отобр. кадрами
	QueueFamilyIndices findQueueFamily(VkPhysicalDevice *pDevice, VkSurfaceKHR *pSurface);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice *pDevice, VkSurfaceKHR *pSurface);
	bool isDeviceSuitable(VkPhysicalDevice *pDevice, VkSurfaceKHR *pSurface);
	bool checkDeviceExtensionSupport(VkPhysicalDevice *pDevice);
	bool checkValidationLayerSupport();

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avilableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
public:
	void setupDebugCallback();
	cGraphic(cWindow *pWindow) :gInstance(VK_NULL_HANDLE), 
								gSurface(VK_NULL_HANDLE),
								gPhysicalDevice(VK_NULL_HANDLE),
								gDevice(VK_NULL_HANDLE),
								gQueue(VK_NULL_HANDLE),
								gCommandBuffer(VK_NULL_HANDLE),
								gFrameBuffer(VK_NULL_HANDLE),
								gRenderPass(VK_NULL_HANDLE),
								pWnd(pWindow) {}
	~cGraphic();

	bool initInstance();
	bool initPhysicalDevice();
	bool initLogicalDevice();
	bool initSurface();

	bool initSwapChain();
	bool initDeviceQueue();
	bool initFramebuffer();


	void presentStep();
};


#endif // !GRAPHICS_H
