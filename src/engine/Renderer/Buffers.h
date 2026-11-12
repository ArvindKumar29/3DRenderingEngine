#pragma once
#include "Vertex.h"
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_raii.hpp>

namespace VKRENDERER
{
	struct vertexBuffer
	{
		VkBuffer buffer = VK_NULL_HANDLE; // RAII hata diya
		VmaAllocation allocation = VK_NULL_HANDLE;
		std::vector<Vertex> VB;
		// Yahan function signature mein VmaAllocator paas kar
		void createVertexBuffer(vk::raii::Device& ld, vk::raii::PhysicalDevice& pd, VmaAllocator allocator) {
			VB = {
				{ { -0.5f,  0.5f,  0.5f },{ 0.0f, 0.0f, 1.0f } },	// 0: Bottom-Left-Front  (Blue)
				{ { 0.5f,  0.5f,  0.5f },{ 1.0f, 0.0f, 1.0f } },	// 1: Bottom-Right-Front (Magenta)
				{ { -0.5f, -0.5f,  0.5f },{ 0.0f, 1.0f, 1.0f } },	// 2: Top-Left-Front     (Cyan)
				{ { 0.5f, -0.5f,  0.5f },{ 1.0f, 1.0f, 1.0f } },	// 3: Top-Right-Front    (White)

				// BACK FACE (Z = -0.5)
				{ { -0.5f,  0.5f, -0.5f },{ 0.0f, 0.0f, 0.0f } },	// 4: Bottom-Left-Back   (Black)
				{ { 0.5f,  0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f } },	// 5: Bottom-Right-Back  (Red)
				{ { -0.5f, -0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f } },	// 6: Top-Left-Back      (Green)
				{ { 0.5f, -0.5f, -0.5f },{ 1.0f, 1.0f, 0.0f } }		// 7: Top-Right-Back     (Yellow)
			};
			VkDeviceSize bufferSize = sizeof(VB[0]) * VB.size();

			VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
			bufferInfo.size = bufferSize;
			bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo allocInfo = {};
			allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
			allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

			VmaAllocationInfo allocResultInfo;
			vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, &allocResultInfo);

			// Memory copy karna ab bohot aasan hai kyunki MAPPED_BIT use kiya hai
			memcpy(allocResultInfo.pMappedData, VB.data(), (size_t)bufferSize);
		}

		void destroy(VmaAllocator allocator) {
			if (buffer != VK_NULL_HANDLE) {
				vmaDestroyBuffer(allocator, buffer, allocation);
			}
		}
	};

	struct indexBuffer
	{
		VkBuffer buffer = VK_NULL_HANDLE; // RAII hata diya
		VmaAllocation allocation = VK_NULL_HANDLE;
		std::vector<uint32_t> IB;

		// Yahan function signature mein VmaAllocator paas kar
		void createIndexBuffer(vk::raii::Device& ld, vk::raii::PhysicalDevice& pd, VmaAllocator allocator) {
			IB = {
				// Front face
				0, 1, 2,  2, 1, 3,
				// Back face
				5, 4, 7,  7, 4, 6,
				// Left face
				4, 0, 6,  6, 0, 2,
				// Right face
				1, 5, 3,  3, 5, 7,
				// Top face
				2, 3, 6,  6, 3, 7,
				// Bottom face
				4, 5, 0,  0, 5, 1
			};
			VkDeviceSize bufferSize = sizeof(IB[0]) * IB.size();

			VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
			bufferInfo.size = bufferSize;
			bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo allocInfo = {};
			allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
			allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

			VmaAllocationInfo allocResultInfo;
			vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, &allocResultInfo);

			// Memory copy karna ab bohot aasan hai kyunki MAPPED_BIT use kiya hai
			memcpy(allocResultInfo.pMappedData, IB.data(), (size_t)bufferSize);
		}

		void destroy(VmaAllocator allocator) {
			if (buffer != VK_NULL_HANDLE) {
				vmaDestroyBuffer(allocator, buffer, allocation);
			}
		}
	};
}