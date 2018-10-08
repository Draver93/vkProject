#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#define VK_USE_PLATFORM_WIN32_KHR


#include <Windows.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "shaderc/shaderc.hpp"
#include "vulkan/vulkan.h"
#include "vulkan/vk_sdk_platform.h"

#define GLM_FORCE_RADIANS
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

#include "glslang/SPIRV/GlslangToSpv.h"
#include "glslang/glslang/OSDependent/osinclude.h"

struct QueueFamilyIndices 
{
	int graphicsFamily = -1;
	int presentFamily = -1;

	bool isComplete() 
	{
		return graphicsFamily >= 0 && presentFamily >= 0;
	}
};
struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct sData {
	float width, height;
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;
	VkQueue graphicsQueue;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;

	VkQueue presentQueue;

	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;

	std::vector<const char*> instance_layer_names;
	std::vector<const char*> instance_extension_names;
	std::vector<const char*> device_extension_names;
};

class cGraphics
{
private:
	HWND hWnd;
	HINSTANCE hInstance;

	sData data;
	bool isDeviceSuitable(VkPhysicalDevice device) 
	{
		QueueFamilyIndices indices = findQueueFamilies(device);
		return indices.isComplete();
	}

	VkShaderModule createShaderModule(const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(data.logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}

	static const char * readFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::string *dataStrings = new std::string();
		file.seekg(0);
		char c;
		while (file.get(c))
		{
			std::cout << c;
			dataStrings->push_back(c);
		}

		file.close();

		return dataStrings->data();
	}
	EShLanguage FindLanguage(const VkShaderStageFlagBits shader_type) 
	{
		switch (shader_type) {
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
			return EShLangVertex;
		}
	}
	VkShaderModule loadShader(const std::string& filename, VkShaderStageFlagBits flag)
	{
		glslang::InitializeProcess();
		EShLanguage eshlLanguage = FindLanguage(flag);
		TBuiltInResource Resources;
		{
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

		const char* vertShaderCode = readFile(filename);

		glslang::TProgram program;
		glslang::TShader shader(eshlLanguage);

		const char *shaderStrings[1];
		shaderStrings[0] = vertShaderCode;
		shader.setStrings(shaderStrings, 1);
		EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
		bool shaderInit = shader.parse(&Resources, 100, false, messages);
		if (!shaderInit)
			return NULL;
		program.addShader(&shader);
		bool programLink = program.link(messages);
		if (!programLink) 
			return NULL;

		std::vector<unsigned int> code;
		glslang::GlslangToSpv(*program.getIntermediate(eshlLanguage), code);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pNext = NULL;
		createInfo.flags = 0;
		createInfo.codeSize = code.size() * sizeof(unsigned int);
		createInfo.pCode = code.data();

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(data.logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}
		glslang::FinalizeProcess();
		delete vertShaderCode;

		return shaderModule;
	}

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport();
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void createInstance();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createRenderPass();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffers();
	void createSemaphores();

public:
	cGraphics(HWND hWnd, HINSTANCE hInstance);
	~cGraphics();
	void drawFrame();
	sData *getData() { return &data; }
};

#endif // !GRAPHICS_HPP
