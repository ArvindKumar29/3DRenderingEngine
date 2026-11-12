#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <volk.h>
#include "../Vulkan/Device.h"
#include "../Vulkan/Window.h"
#include "Camera.h"

namespace VKRENDERER
{
	class UI
	{
	public:
		UI();
		~UI();
		void init(Window& window, Device& device, vk::raii::Instance& instance);
		void renderFrame(Camera& camera, vk::DescriptorSet& depthTextureID);

		vk::raii::DescriptorPool pool = nullptr;
	private:
		VkFormat colorAttachmentFormat = VK_FORMAT_UNDEFINED;
		ImGui_ImplVulkan_PipelineInfo pipelineInfo = {};
	};
}