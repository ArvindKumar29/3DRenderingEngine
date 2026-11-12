#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <fstream>
#include "spdlog/spdlog.h"
#include "../Renderer/Vertex.h"

namespace VKRENDERER
{
	class Pipeline
	{

		vk::raii::ShaderModule createShaderModule(vk::raii::Device& ld, const std::vector<char>& code);


	public:
		Pipeline();
		~Pipeline();
		void createGraphicsPipeline(vk::raii::Device& ld, vk::Format surfaceFormat, vk::Format depthFormat);
		std::vector<char> readFile(const std::string& fileName);
		void createSyncObjects(vk::raii::Device& ld);

		vk::raii::PipelineLayout graphicsPipeLineLayout = nullptr;
		vk::raii::Pipeline graphicsPipeline = nullptr;
		vk::raii::Semaphore imageAvailableSemaphores = nullptr;
		vk::raii::Semaphore renderFinishedSemaphore	 = nullptr;
		vk::raii::Fence		renderFence				 = nullptr;
	};
}