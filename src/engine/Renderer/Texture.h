#pragma once
#include <stb_image.h>
#include <string>
#include <stdexcept>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_raii.hpp>
#include "../Vulkan/Device.h"
#include "../Vulkan/Window.h"


namespace VKRENDERER
{
	class Texture
	{
	public:
		Texture();
		~Texture();

		void loadFromFile(const char* filePath, Device& device, vk::raii::CommandBuffer& cmdBuffer, Window& window, VmaAllocator& allocator);

		int width = 0, height = 0, channels = 0;
		uint32_t imageSize = 0;                 
		unsigned char* pixels = nullptr;

		VkImage image = nullptr;
		VmaAllocation allocation = nullptr;
		vk::raii::ImageView imageView = nullptr;
		vk::raii::Sampler sampler = nullptr;

	};
}