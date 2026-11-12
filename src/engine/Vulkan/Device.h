#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <spdlog/spdlog.h>
//#include "engine/Engine.h"

namespace VKRENDERER
{
	class Device
	{

		bool hasDGPU = false;
		bool isDeviceSuitable(vk::raii::PhysicalDevice& const pd, vk::raii::SurfaceKHR& surface, std::vector<char const*> requiredDeviceExtensions);
	public:
		Device();
		~Device();

		void getPhysicalDevice(vk::raii::Instance& instance, vk::raii::SurfaceKHR& surface, std::vector<char const*> requiredDeviceExtensions);
		void createLogicalDevice(std::vector<char const*> requiredDeviceExtensions);
		uint32_t findMemoryType(vk::raii::PhysicalDevice& pd, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
		
		vk::raii::PhysicalDevice physicalDevice = nullptr;
		vk::raii::Device		 logicalDevice = nullptr;
		vk::raii::Queue			 graphicsQueue = nullptr;
		uint32_t graphicsQueueIndex;
	};
}