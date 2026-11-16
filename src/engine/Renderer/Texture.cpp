#include "Texture.h"

namespace VKRENDERER 
{
	Texture::Texture()
	{}

	Texture::~Texture()
	{}

	void Texture::loadFromFile(const char* filePath, Device& device, vk::raii::CommandBuffer& cmdBuffer, Window& window, VmaAllocator& allocator)
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

		vk::ImageCreateInfo imageCreateInfo = vk::ImageCreateInfo()
			.setImageType(vk::ImageType::e2D)
			.setExtent(vk::Extent3D(width, height, 1))
			.setFormat(vk::Format::eR8G8B8A8Srgb)
			.setTiling(vk::ImageTiling::eOptimal)
			.setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
			.setMipLevels(1)
			.setArrayLayers(1)
			.setSamples(vk::SampleCountFlagBits::e1);

		VmaAllocationCreateInfo imageAllocInfo = {};
		imageAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		vmaCreateImage(allocator, imageCreateInfo, &imageAllocInfo, &image, &allocation, nullptr);

		vk::ImageViewCreateInfo imageViewCreateInfo = vk::ImageViewCreateInfo()
			.setViewType(vk::ImageViewType::e2D)
			.setImage(image)
			.setFormat(vk::Format::eR8G8B8A8Srgb)
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setLayerCount(1)
				.setBaseArrayLayer(0)
			);

		imageView = vk::raii::ImageView(device.logicalDevice, imageViewCreateInfo);

		// Sampler
		vk::SamplerCreateInfo samplerInfo = vk::SamplerCreateInfo()
			.setMagFilter(vk::Filter::eLinear)
			.setMinFilter(vk::Filter::eLinear)
			.setAddressModeU(vk::SamplerAddressMode::eRepeat)
			.setAddressModeV(vk::SamplerAddressMode::eRepeat)
			.setAddressModeW(vk::SamplerAddressMode::eRepeat)
			.setAnisotropyEnable(vk::False)
			.setBorderColor(vk::BorderColor::eIntOpaqueBlack)
			.setUnnormalizedCoordinates(vk::False)
			.setCompareEnable(vk::False)
			.setMipmapMode(vk::SamplerMipmapMode::eLinear);

		sampler = device.logicalDevice.createSampler(samplerInfo);

		vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo()
			.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		cmdBuffer.begin(beginInfo);

		window.transitionImageLayout(cmdBuffer, image, 
			vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 
			vk::AccessFlagBits2::eNone, vk::AccessFlagBits2::eTransferWrite, 
			vk::PipelineStageFlagBits2::eTopOfPipe, vk::PipelineStageFlagBits2::eTransfer);

		vk::BufferImageCopy imageCopy = vk::BufferImageCopy()
			.setImageSubresource(vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1))
			.setImageExtent(vk::Extent3D(width, height, 1));

		cmdBuffer.copyBufferToImage(stagingBuffer, image, vk::ImageLayout::eTransferDstOptimal, imageCopy);

		window.transitionImageLayout(cmdBuffer, image,
			vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
			vk::AccessFlagBits2::eTransferWrite, vk::AccessFlagBits2::eShaderRead,
			vk::PipelineStageFlagBits2::eTransfer, vk::PipelineStageFlagBits2::eFragmentShader);

		cmdBuffer.end();

		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

		vk::SubmitInfo submitInfo = vk::SubmitInfo()
			.setCommandBuffers(*cmdBuffer);

		device.graphicsQueue.submit(submitInfo);
		device.graphicsQueue.waitIdle();

		vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);
	}
}
