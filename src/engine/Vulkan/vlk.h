#pragma once
#include "Instance.h"
#include "ValidationLayers.h"
#include "Window.h"
#include "Device.h"
#include <vulkan/vulkan_raii.hpp>
#include <imgui_impl_vulkan.h>
#include <glm/glm.hpp>
#include "Pipeline.h"
#include "CommandPool.h"
#include <spdlog/spdlog.h>
#include "../Renderer/Buffers.h"
#include "../Renderer/Input.h"
#include "../Renderer/Vertex.h"
#include "../Renderer/Camera.h"
#include "../Renderer/UI.h"
#include "../Time.h"
#include "vma.h"

namespace VKRENDERER
{
	class vlk
	{
	public:
		vlk();
		~vlk();
		void run();

	private:
		void init();
		void renderFrame();

		// Vulkan Core
		GLFWwindow* m_window;
		vk::raii::Context ctx;
		vk::raii::SurfaceKHR* m_Surface = nullptr;
		vk::raii::SwapchainKHR* m_SwapChain = nullptr;
		vk::raii::PhysicalDevice pd = nullptr;
		vk::raii::Pipeline* m_GraphicsPipeline = nullptr;
		vk::raii::CommandPool* m_cmdPool = nullptr;
		vk::raii::CommandBuffer* m_cmdBuffer = nullptr;
		vk::raii::Sampler depthSampler = nullptr;
		Window window;
		ValidationLayers validationLayers;
		Instance vulkanInstance;
		Device device;
		vertexBuffer vb;
		indexBuffer ib;

		// Renderer Stuff
		Pipeline pipeline;
		CommandPool cmdPool;
		Input inputSystem;
		Camera camera;
		Time time;
		UI ui;

		struct pushConstantData
		{
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
		} pc;

		glm::mat4 modelMatrix = glm::mat4(1.0f);

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
		};

		vk::DescriptorSet depthTextureID = nullptr;

		VmaAllocator allocator = nullptr;
	};
}