#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <spdlog/spdlog.h>
#include "../Renderer/Buffers.h"
#include <imgui.h>
#include <imgui_impl_vulkan.h>

namespace VKRENDERER
{
	class CommandPool
	{
	public:
		CommandPool();
		~CommandPool();
		vk::raii::CommandPool cmdPool = nullptr;
		vk::raii::CommandBuffer cmdBuffer = nullptr;

		void createCommandPool(vk::raii::Device& ld, uint32_t queueIndex);
		void creatCommandBuffer(vk::raii::Device& ld);
		void recordCommandBuffer(vk::raii::Device& ld, vk::raii::PhysicalDevice& pd, vk::ImageView imageView, vk::Extent2D extent, vk::raii::Pipeline& pipeline, vk::raii::PipelineLayout& pipelineLayout, vertexBuffer& vertexBuffer, indexBuffer& indexBuffer, glm::mat4 view, glm::mat4 proj, glm::mat4 model);
	};
}