#include "UI.h"

namespace VKRENDERER
{
	UI::UI()
	{}

	UI::~UI()
	{}

	void UI::init(Window& window, Device& device, vk::raii::Instance& instance)
	{
		// Initialize ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForVulkan(window.window, true);

		// Create Descriptor Pool for ImGui
		std::array<vk::DescriptorPoolSize, 11> poolSizes = {
			vk::DescriptorPoolSize(vk::DescriptorType::eSampler, 1000),
			vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 1000),
			vk::DescriptorPoolSize(vk::DescriptorType::eSampledImage, 1000),
			vk::DescriptorPoolSize(vk::DescriptorType::eStorageImage, 1000),
			vk::DescriptorPoolSize(vk::DescriptorType::eUniformTexelBuffer, 1000),
			vk::DescriptorPoolSize(vk::DescriptorType::eStorageTexelBuffer, 1000),
			vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1000),
			vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 1000),
			vk::DescriptorPoolSize(vk::DescriptorType::eUniformBufferDynamic, 1000),
			vk::DescriptorPoolSize(vk::DescriptorType::eStorageBufferDynamic, 1000),
			vk::DescriptorPoolSize(vk::DescriptorType::eInputAttachment, 1000)
		};

		vk::DescriptorPoolCreateInfo initInfo = vk::DescriptorPoolCreateInfo()
			.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
			.setMaxSets(1000 * static_cast<uint32_t>(poolSizes.size()))
			.setPoolSizes(poolSizes);

		pool = device.logicalDevice.createDescriptorPool(initInfo);

		// Initialize ImGui Vulkan backend
		ImGui_ImplVulkan_InitInfo initInfoVulkan = {};
		initInfoVulkan.ApiVersion = VK_API_VERSION_1_4;
		initInfoVulkan.Instance = static_cast<VkInstance>(*instance);
		initInfoVulkan.PhysicalDevice = static_cast<VkPhysicalDevice>(*device.physicalDevice);
		initInfoVulkan.Device = static_cast<VkDevice>(*device.logicalDevice);
		initInfoVulkan.QueueFamily = device.graphicsQueueIndex;
		initInfoVulkan.Queue = static_cast<VkQueue>(*device.graphicsQueue);
		initInfoVulkan.PipelineCache = VK_NULL_HANDLE;
		initInfoVulkan.DescriptorPool = *pool;

		const uint32_t swapChainImageCount = static_cast<uint32_t>(window.swapChainImages.size());
		if (swapChainImageCount < 2)
			throw std::runtime_error("ImGui Vulkan backend requires at least two swapchain images");
		initInfoVulkan.MinImageCount = window.swapChainMinImageCount;
		initInfoVulkan.ImageCount = swapChainImageCount;

		// DYNAMIC RENDERING
		initInfoVulkan.UseDynamicRendering = true;
		colorAttachmentFormat = static_cast<VkFormat>(window.swapChainSurfaceFormat.format);

		pipelineInfo = {};
		pipelineInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		pipelineInfo.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
		pipelineInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
		pipelineInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats = &colorAttachmentFormat;
		pipelineInfo.PipelineRenderingCreateInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
		pipelineInfo.PipelineRenderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

		initInfoVulkan.PipelineInfoMain = pipelineInfo;

		if (!ImGui_ImplVulkan_Init(&initInfoVulkan))
		{
			throw std::runtime_error("Failed to initialize ImGui Vulkan backend");
		}
	}

	void UI::renderFrame(Camera& camera, vk::DescriptorSet& depthTextureID)
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Debug Menu");
		ImGui::Text("Rendering Engine Debug Menu");
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
		ImGui::Separator();
		ImGui::Text("Depth Texture ID: %p", static_cast<void*>(depthTextureID.operator VkDescriptorSet()));
		ImGui::Image((ImTextureID)static_cast<VkDescriptorSet>(depthTextureID), ImVec2(300, 300), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::End();
		ImGui::Render();
	}
}