#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <cassert> 
#include <cstdlib>
#include <vector>

#define VK_USE_PLATFORM_WIN32_KHR

#include "vulkan/vulkan.h"
#include <vulkan/vk_sdk_platform.h>

#include "Window.hpp"

class cGraphic
{
private:
	cWindow *pWnd;

	VkInstance gInstance;
	VkSurfaceKHR gSurface;
	VkPhysicalDevice gPhysicalDevice;
	VkDevice gDevice;
	VkSwapchainKHR gSwapChain;
	VkExtent2D surfaceResolution;
	VkQueue gPresentQueue;
	VkCommandBuffer gCommandBuffer;

	VkFramebuffer *gFrameBuffer;
	VkRenderPass gRenderPass;

	std::vector<VkImage> gPresentImages; //Отображаемые кадры
	std::vector<VkImageView> gImageViews; //указатели через которые можно управлять отобр. кадрами
public:
	cGraphic(cWindow *pWindow) :gInstance(VK_NULL_HANDLE), 
								gSurface(VK_NULL_HANDLE),
								gPhysicalDevice(VK_NULL_HANDLE),
								gDevice(VK_NULL_HANDLE),
								gPresentQueue(VK_NULL_HANDLE),
								gCommandBuffer(VK_NULL_HANDLE),
								gFrameBuffer(VK_NULL_HANDLE),
								gRenderPass(VK_NULL_HANDLE),
								pWnd(pWindow) {}
	~cGraphic();

	bool initInstance();
	bool initSurface();
	bool initPhysicalDevice();
	bool initLogicalDevice();
	bool initSwapChain();
	bool initDeviceQueue();
	bool initFramebuffer();


	void presentStep();
};


#endif // !GRAPHICS_H
