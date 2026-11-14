#pragma once
#include <stb_image.h>
#include <string>
#include <stdexcept>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_raii.hpp>


namespace VKRENDERER
{
	class Texture
	{
	public:
		Texture();
		~Texture();

		void loadFromFile(const char* filePath, VmaAllocator& allocator);

		int width = 0, height = 0, channels = 0;
		uint32_t imageSize = 0;                 
		unsigned char* pixels = nullptr;


	private:


	};
}