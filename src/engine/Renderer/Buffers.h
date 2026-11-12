#pragma once
#include "Vertex.h"
#include <vulkan/vulkan_raii.hpp>

namespace VKRENDERER
{
	struct vertexBuffer
	{
		
		std::vector<Vertex> VB = {
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

		
		vk::raii::Buffer buffer = nullptr;
		vk::raii::DeviceMemory bufferMemory = nullptr;

		void createVertexBuffer(vk::raii::Device& ld, vk::raii::PhysicalDevice& pd)
		{
			vk::DeviceSize bufferSize = sizeof(VB[0]) * VB.size();

			vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo()
				.setSize(bufferSize)
				.setUsage(vk::BufferUsageFlagBits::eVertexBuffer)
				.setSharingMode(vk::SharingMode::eExclusive);
			buffer = vk::raii::Buffer(ld, bufferInfo);

			vk::MemoryRequirements memReq = buffer.getMemoryRequirements();
			vk::PhysicalDeviceMemoryProperties memProp = pd.getMemoryProperties();
			vk::MemoryPropertyFlags requiredProps = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

			uint32_t memoryTypeIndex = 0;
			for (uint32_t i = 0; i < memProp.memoryTypeCount; i++)
			{
				if ((memReq.memoryTypeBits & (1 << i)) && ((memProp.memoryTypes[i].propertyFlags & requiredProps) == requiredProps))
				{
					memoryTypeIndex = i;
					break;
				}
			}

			vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
				.setAllocationSize(memReq.size)
				.setMemoryTypeIndex(memoryTypeIndex);

			bufferMemory = vk::raii::DeviceMemory(ld, allocInfo);
			buffer.bindMemory(*bufferMemory, 0);

			void* data = bufferMemory.mapMemory(0, bufferSize);
			memcpy(data, VB.data(), static_cast<size_t>(bufferSize));
			bufferMemory.unmapMemory();
		}
	};

	struct indexBuffer
	{
		std::vector<uint32_t> IB = {
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
		vk::raii::Buffer buffer = nullptr;
		vk::raii::DeviceMemory bufferMemory = nullptr;

		void createIndexBuffer(vk::raii::Device& ld, vk::raii::PhysicalDevice& pd)
		{
			vk::DeviceSize bufferSize = sizeof(IB[0]) * IB.size();

			vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo()
				.setSize(bufferSize)
				.setUsage(vk::BufferUsageFlagBits::eIndexBuffer)
				.setSharingMode(vk::SharingMode::eExclusive);
			buffer = vk::raii::Buffer(ld, bufferInfo);

			vk::MemoryRequirements memReq = buffer.getMemoryRequirements();
			vk::PhysicalDeviceMemoryProperties memProp = pd.getMemoryProperties();
			vk::MemoryPropertyFlags requiredProps = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

			uint32_t memoryTypeIndex = 0;
			for (uint32_t i = 0; i < memProp.memoryTypeCount; i++)
			{
				if ((memReq.memoryTypeBits & (1 << i)) && ((memProp.memoryTypes[i].propertyFlags & requiredProps) == requiredProps))
				{
					memoryTypeIndex = i;
					break;
				}
			}

			vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
				.setAllocationSize(memReq.size)
				.setMemoryTypeIndex(memoryTypeIndex);

			bufferMemory = vk::raii::DeviceMemory(ld, allocInfo);
			buffer.bindMemory(*bufferMemory, 0);

			void* data = bufferMemory.mapMemory(0, bufferSize);
			memcpy(data, IB.data(), static_cast<size_t>(bufferSize));
			bufferMemory.unmapMemory();
		}
	};
}