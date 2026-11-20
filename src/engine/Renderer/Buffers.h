#pragma once
#include "Vertex.h"
#include "../Vulkan/Device.h"
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_raii.hpp>

namespace VKRENDERER
{
	struct vertexBuffer
	{
		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;
		void createVertexBuffer(Device& device, const std::vector<Vertex>& VB, vk::raii::CommandBuffer& cmdBuffer, VmaAllocator allocator) {
			if (VB.empty())
				return;

			VkDeviceSize bufferSize = sizeof(VB[0]) * VB.size();
			
			// Staging buffer
			VkBuffer stagingBuffer = VK_NULL_HANDLE;
			VmaAllocation stagingAlloc = VK_NULL_HANDLE;
			VmaAllocationInfo stagingInfo = {};

			VkBufferCreateInfo stagingInfoCreate = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
			stagingInfoCreate.size = bufferSize;
			stagingInfoCreate.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			stagingInfoCreate.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo stagingAllocCreate = {};
			stagingAllocCreate.usage = VMA_MEMORY_USAGE_AUTO;
			stagingAllocCreate.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

			vmaCreateBuffer(allocator, &stagingInfoCreate, &stagingAllocCreate,
				&stagingBuffer, &stagingAlloc, &stagingInfo);
			memcpy(stagingInfo.pMappedData, VB.data(), static_cast<size_t>(bufferSize));


			VkBufferCreateInfo gpuBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
			gpuBufferInfo.size = bufferSize;
			gpuBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			gpuBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo gpuAllocCreate = {};
			gpuAllocCreate.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

			vmaCreateBuffer(allocator, &gpuBufferInfo, &gpuAllocCreate,
				&buffer, &allocation, nullptr);


			vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo()
				.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
			cmdBuffer.begin(beginInfo);
			vk::BufferCopy copyRegion{};
			copyRegion.setSize(bufferSize);
			cmdBuffer.copyBuffer(stagingBuffer, buffer, copyRegion);
			cmdBuffer.end();
		
			vk::SubmitInfo submitInfo{};
			submitInfo.setCommandBuffers(*cmdBuffer);

			device.graphicsQueue.submit(submitInfo, nullptr);
			device.graphicsQueue.waitIdle();

			vmaDestroyBuffer(allocator, stagingBuffer, stagingAlloc);
		
		}

		void destroy(VmaAllocator allocator) {
			if (buffer != VK_NULL_HANDLE) {
				vmaDestroyBuffer(allocator, buffer, allocation);
			}
		}
	};

	struct indexBuffer
	{
		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;

		void createindexBuffer(Device& device, const std::vector<uint32_t>& IB, vk::raii::CommandBuffer& cmdBuffer, VmaAllocator allocator) {
			if (IB.empty())
				return;

			VkDeviceSize bufferSize = sizeof(IB[0]) * IB.size();

			// Staging buffer
			VkBuffer stagingBuffer = VK_NULL_HANDLE;
			VmaAllocation stagingAlloc = VK_NULL_HANDLE;
			VmaAllocationInfo stagingInfo = {};

			VkBufferCreateInfo stagingInfoCreate = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
			stagingInfoCreate.size = bufferSize;
			stagingInfoCreate.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			stagingInfoCreate.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo stagingAllocCreate = {};
			stagingAllocCreate.usage = VMA_MEMORY_USAGE_AUTO;
			stagingAllocCreate.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

			vmaCreateBuffer(allocator, &stagingInfoCreate, &stagingAllocCreate,
				&stagingBuffer, &stagingAlloc, &stagingInfo);
			memcpy(stagingInfo.pMappedData, IB.data(), static_cast<size_t>(bufferSize));


			VkBufferCreateInfo gpuBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
			gpuBufferInfo.size = bufferSize;
			gpuBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			gpuBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo gpuAllocCreate = {};
			gpuAllocCreate.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

			vmaCreateBuffer(allocator, &gpuBufferInfo, &gpuAllocCreate,
				&buffer, &allocation, nullptr);


			vk::CommandBufferBeginInfo beginInfo{};
			beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
			cmdBuffer.begin(beginInfo);
			vk::BufferCopy copyRegion{};
			copyRegion.setSize(bufferSize);
			cmdBuffer.copyBuffer(stagingBuffer, buffer, copyRegion);
			cmdBuffer.end();

			vk::SubmitInfo submitInfo{};
			submitInfo.setCommandBuffers(*cmdBuffer);

			device.graphicsQueue.submit(submitInfo, nullptr);
			device.graphicsQueue.waitIdle();

			vmaDestroyBuffer(allocator, stagingBuffer, stagingAlloc);

		}

		void destroy(VmaAllocator allocator) {
			if (buffer != VK_NULL_HANDLE) {
				vmaDestroyBuffer(allocator, buffer, allocation);
			}
		}
	};
}