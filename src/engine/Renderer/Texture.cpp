#include "Texture.h"

namespace VKRENDERER 
{
	Texture::Texture()
	{}

	Texture::~Texture()
	{}

	void Texture::loadFromFile(const char* filePath, VmaAllocator& allocator)
	{
		pixels = stbi_load(filePath, &width, &height, &channels, STBI_rgb_alpha);
		if (!pixels)
			throw std::runtime_error("Failed to laod Texture...");
		imageSize = width * height * 4;
		
		vk::BufferCreateInfo vkBufferCreateInfo = vk::BufferCreateInfo()
			.setSize(imageSize)
			.setUsage(vk::BufferUsageFlagBits::eTransferSrc)
			.setSharingMode(vk::SharingMode::eExclusive);

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VkBuffer stagingBuffer = VK_NULL_HANDLE;
		VmaAllocation stagingAllocation = VK_NULL_HANDLE;
		VmaAllocationInfo stagingAllocInfo = {};

		vmaCreateBuffer(allocator, vkBufferCreateInfo, &allocInfo, &stagingBuffer, &stagingAllocation, &stagingAllocInfo);

		memcpy(stagingAllocInfo.pMappedData, pixels, imageSize);
		stbi_image_free(pixels);
	}
}
