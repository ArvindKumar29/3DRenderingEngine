#pragma once
//#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//#include <math.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan_raii.hpp>
#include <spdlog/spdlog.h>
#include "Device.h"
#include <vma/vk_mem_alloc.h>

namespace VKRENDERER
{
	class Window
	{

		vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const &availableFormats);
		vk::PresentModeKHR choosePresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes);
		vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const &capabilities, GLFWwindow* window);
		uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const &capabilities);


	public:
		Window();
		~Window();

		GLFWwindow* initWindow(uint32_t width, uint32_t height, const char* title);
		void initSurface(vk::raii::Instance& instance, GLFWwindow* window);
		void createSwapChain(vk::raii::PhysicalDevice& pd, vk::raii::Device& ld);
		void createImageViews(vk::raii::Device& ld);
		void createDepthResources(Device& device, VmaAllocator allocator);
		void transitionImageLayout(vk::raii::CommandBuffer& cmdBuffer, 
			vk::Image image, 
			vk::ImageLayout oldLayout, 
			vk::ImageLayout newLayout, 
			vk::AccessFlags2 srcAccess, 
			vk::AccessFlags2 dstAccess, 
			vk::PipelineStageFlags2 srcStage, 
			vk::PipelineStageFlags2 dstStage, 
			vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eColor);
	
		GLFWwindow* window;
		vk::raii::SurfaceKHR surface = nullptr;
		vk::raii::SwapchainKHR swapChain = nullptr;
		vk::SurfaceFormatKHR swapChainSurfaceFormat;
		uint32_t swapChainMinImageCount = 0;
		std::vector<vk::raii::ImageView> swapChainImageViews;
		vk::Extent2D                     swapChainExtent;
		std::vector<vk::Image>           swapChainImages;

		// Depth Format
		vk::Format depthFormat;
		VkImage depthImage = nullptr;
		vk::raii::DeviceMemory depthImageMemory = nullptr;
		vk::raii::ImageView depthImageView = nullptr;
		vk::raii::ImageView depthSampleView = nullptr;
		VmaAllocation depthImageAllocation = nullptr;
	};
}
