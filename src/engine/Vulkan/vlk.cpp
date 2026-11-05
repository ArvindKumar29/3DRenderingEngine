#include "vlk.h"
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace VKRENDERER
{
	vlk::vlk()
	{}
	
	vlk::~vlk()
	{
		device.logicalDevice.waitIdle();

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		ui.pool = nullptr;

		cmdPool.cmdBuffer = nullptr;
		cmdPool.cmdPool = nullptr;

		pipeline.imageAvailableSemaphores = nullptr;
		pipeline.renderFinishedSemaphore = nullptr;
		pipeline.renderFence = nullptr;

		pipeline.graphicsPipeline = nullptr;
		pipeline.graphicsPipeLineLayout = nullptr;

		window.swapChainImageViews.clear();
		window.swapChain = nullptr;

		vb.buffer = nullptr;
		vb.bufferMemory = nullptr;
		ib.buffer = nullptr;
		ib.bufferMemory = nullptr;

		device.graphicsQueue = nullptr;
		device.logicalDevice = nullptr;
		device.physicalDevice = nullptr;
		pd = nullptr;

		window.surface = nullptr;

		vulkanInstance.instance = nullptr;

		if (m_window)
		{
			glfwDestroyWindow(m_window);
			m_window = nullptr;
		}
		glfwTerminate();
	}

	void vlk::init()
	{
		// Creating Window
		uint32_t width = 1920, height = 1080;
		const char* title = "Rendering Engine";
		m_window = window.initWindow(width, height, title);
		inputSystem.Init(m_window);

		// Creating Vulkan Instance and Setting up Validation Layers
		vulkanInstance.init(ctx);

		// Creating surface for rendering
		window.initSurface(vulkanInstance.instance, m_window);
		m_Surface = &window.surface;

		// choosing appropriate physical device...
		device.getPhysicalDevice(vulkanInstance.instance, window.surface, deviceExtensions);
		pd = device.physicalDevice;
		vk::PhysicalDeviceProperties pdProperties = pd.getProperties();
		spdlog::info("Selected Device Info:");
		spdlog::info("Name: {}", pdProperties.deviceName.data());
		spdlog::info("Type: {}", pdProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu);
		spdlog::info("Vulkan API Version: {}.{}.{}",
			VK_VERSION_MAJOR(pdProperties.apiVersion),
			VK_VERSION_MINOR(pdProperties.apiVersion),
			VK_VERSION_PATCH(pdProperties.apiVersion));

		//spdlog::info("Creating Logical Device...");
		device.createLogicalDevice(deviceExtensions);
		window.createSwapChain(device.physicalDevice, device.logicalDevice);
		m_SwapChain = &window.swapChain;

		window.createImageViews(device.logicalDevice);

		pipeline.createGraphicsPipeline(device.logicalDevice, window.swapChainSurfaceFormat.format);
		m_GraphicsPipeline = &pipeline.graphicsPipeline;

		cmdPool.createCommandPool(device.logicalDevice, device.graphicsQueueIndex);
		cmdPool.creatCommandBuffer(device.logicalDevice);
		m_cmdBuffer = &cmdPool.cmdBuffer;
		m_cmdPool = &cmdPool.cmdPool;

		vb.createVertexBuffer(device.logicalDevice, device.physicalDevice);
		ib.createIndexBuffer(device.logicalDevice, device.physicalDevice);

		pipeline.createSyncObjects(device.logicalDevice);

		ui.init(window, device, vulkanInstance.instance);
	}


	void vlk::run()
	{
		init();
		time.updateDeltaTime();
		float dt = time.getDeltaTime();
		while (!glfwWindowShouldClose(m_window))
		{
			inputSystem.updateKeyStates();
			ui.renderFrame(camera);
			glfwPollEvents();
			if (inputSystem.isKeyHeld(GLFW_KEY_ESCAPE))
				glfwSetWindowShouldClose(m_window, true);
			float moveSpeed = 0.05f;
			glm::vec3 camPos = camera.getPosition();
			glm::vec3 camRight = glm::normalize(glm::cross(camera.getFront(), WORLD_UP));

			if (inputSystem.isKeyHeld(GLFW_KEY_W))
				camPos += camera.getFront() * moveSpeed * dt; 
			if (inputSystem.isKeyHeld(GLFW_KEY_S))
				camPos -= camera.getFront() * moveSpeed * dt; 
			if (inputSystem.isKeyHeld(GLFW_KEY_A))
				camPos -= camRight * moveSpeed * dt;     
			if (inputSystem.isKeyHeld(GLFW_KEY_D))
				camPos += camRight * moveSpeed * dt;     
			camera.setPosition(camPos);


			glm::vec2 mouseDelta = inputSystem.isMouseMoved();
			if (mouseDelta != glm::vec2(0.0f, 0.0f))
			{
				float sensitivity = 0.1f;
				glm::vec3 currentRot = camera.getRotation();
				currentRot.y += mouseDelta.x * sensitivity * dt;
				currentRot.x -= mouseDelta.y * sensitivity * dt;

				if (currentRot.x > 89.0f)  currentRot.x = 89.0f;
				if (currentRot.x < -89.0f) currentRot.x = -89.0f;

				camera.setRotation(currentRot);
			}
			camera.update();
			renderFrame();
		}

		device.logicalDevice.waitIdle();

		window.swapChainImageViews.clear();
		window.swapChain = nullptr;
	}

	void vlk::renderFrame()
	{
		device.logicalDevice.waitForFences(*pipeline.renderFence, vk::True, UINT64_MAX);

		auto [result, imageIndex] = window.swapChain.acquireNextImage(UINT64_MAX, *pipeline.imageAvailableSemaphores, nullptr);

		device.logicalDevice.resetFences(*pipeline.renderFence);

		cmdPool.cmdBuffer.reset();
		vk::CommandBufferBeginInfo beginInfo{};
		cmdPool.cmdBuffer.begin(beginInfo);

		// Creating Vertex Buffer
		
		
		window.transitionImageLayout(cmdPool.cmdBuffer, imageIndex, vk::ImageLayout::eUndefined, vk::ImageLayout::eAttachmentOptimal, vk::AccessFlags2(), vk::AccessFlagBits2::eColorAttachmentWrite, vk::PipelineStageFlagBits2::eTopOfPipe, vk::PipelineStageFlagBits2::eColorAttachmentOutput);
		cmdPool.recordCommandBuffer(device.logicalDevice, device.physicalDevice, window.swapChainImageViews[imageIndex], window.swapChainExtent, pipeline.graphicsPipeline, pipeline.graphicsPipeLineLayout, vb, ib, camera.getViewMatrix(), camera.getProjectionMatrix(), modelMatrix);
		window.transitionImageLayout(cmdPool.cmdBuffer, imageIndex, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR, vk::AccessFlagBits2::eColorAttachmentWrite, vk::AccessFlags2(), vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eBottomOfPipe);

		cmdPool.cmdBuffer.end();

		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

		vk::SubmitInfo submitInfo = vk::SubmitInfo()
			.setWaitSemaphores(*pipeline.imageAvailableSemaphores)
			.setWaitDstStageMask(waitStages)
			.setCommandBuffers(*cmdPool.cmdBuffer)
			.setSignalSemaphores(*pipeline.renderFinishedSemaphore);
		device.graphicsQueue.submit(submitInfo, *pipeline.renderFence);

		vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR()
			.setWaitSemaphores(*pipeline.renderFinishedSemaphore)
			.setSwapchains(*window.swapChain)
			.setImageIndices(imageIndex);
		auto presentResult = device.graphicsQueue.presentKHR(presentInfo);
	}
}