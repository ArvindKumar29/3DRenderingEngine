#include "Window.h"

namespace VKRENDERER
{
	Window::Window(){}
	Window::~Window(){}

	GLFWwindow* Window::initWindow(uint32_t width, uint32_t height, const char* title)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		return window;
	}


	void Window::initSurface(vk::raii::Instance& instance, GLFWwindow* window)
	{
		VkSurfaceKHR _surface;
		if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0)
		{
			throw std::runtime_error("Failed to create window surface!!!");
		}
		surface = vk::raii::SurfaceKHR(instance, _surface);
	}

	void Window::createSwapChain(vk::raii::PhysicalDevice& pd, vk::raii::Device& ld)
	{
		auto surfaceCapabilities = pd.getSurfaceCapabilitiesKHR(*surface);
		swapChainExtent = chooseSwapExtent(surfaceCapabilities, window);
		uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);
		swapChainMinImageCount = minImageCount;

		std::vector<vk::SurfaceFormatKHR> availableFormats = pd.getSurfaceFormatsKHR(*surface);
		swapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);

		std::vector<vk::PresentModeKHR> availablePresentModes = pd.getSurfacePresentModesKHR(*surface);
		vk::PresentModeKHR presentMode = choosePresentMode(availablePresentModes);

		vk::SwapchainCreateInfoKHR swapChainCreateInfo = vk::SwapchainCreateInfoKHR()
			.setSurface(*surface)
			.setMinImageCount(minImageCount)
			.setImageFormat(swapChainSurfaceFormat.format)
			.setImageColorSpace(swapChainSurfaceFormat.colorSpace)
			.setImageExtent(swapChainExtent)
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
			.setImageSharingMode(vk::SharingMode::eExclusive)
			.setPreTransform(surfaceCapabilities.currentTransform)
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
			.setPresentMode(presentMode)
			.setClipped(true);

		swapChain = vk::raii::SwapchainKHR(ld, swapChainCreateInfo);
		swapChainImages = swapChain.getImages();
	}

	vk::SurfaceFormatKHR Window::chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats)
	{
		const auto formatIt = std::ranges::find_if(
			availableFormats, [](const auto& format) {return (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear);
			});
		return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
	}

	vk::PresentModeKHR Window::choosePresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes)
	{
		assert(std::ranges::any_of(availablePresentModes, [](auto presentMode) { return presentMode == vk::PresentModeKHR::eFifo; }));
		return std::ranges::any_of(availablePresentModes,
			[](const vk::PresentModeKHR value) { return vk::PresentModeKHR::eMailbox == value; }) ?
			vk::PresentModeKHR::eMailbox :
			vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D Window::chooseSwapExtent(vk::SurfaceCapabilitiesKHR const &capabilities, GLFWwindow* window)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		return {
			std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
			std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
		};
	}

	uint32_t Window::chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& capabilities)
	{
		auto minImageCount = std::max(3u, capabilities.minImageCount);
		if ((0 < capabilities.maxImageCount) && (capabilities.maxImageCount < minImageCount))
		{
			minImageCount = capabilities.maxImageCount;
		}
		return minImageCount;
	}

	void Window::createImageViews(vk::raii::Device& ld)
	{
		assert(swapChainImageViews.empty());

		vk::ImageViewCreateInfo imageViewCreateInfo = vk::ImageViewCreateInfo()
			.setViewType(vk::ImageViewType::e2D)
			.setFormat(swapChainSurfaceFormat.format)
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1));
	
		for (auto& image : swapChainImages)
		{
			imageViewCreateInfo.image = image;
			swapChainImageViews.emplace_back(ld, imageViewCreateInfo);
		}
	}

	void Window::transitionImageLayout(vk::raii::CommandBuffer& cmdBuffer, 
		vk::Image image, 
		vk::ImageLayout old_layout, 
		vk::ImageLayout new_layout, 
		vk::AccessFlags2 src_stage_access, 
		vk::AccessFlags2 dst_stage_access, 
		vk::PipelineStageFlags2 src_stage_mask, 
		vk::PipelineStageFlags2 dst_stage_mask, 
		vk::ImageAspectFlags aspectMask)
	{
		vk::ImageMemoryBarrier2 barrier = vk::ImageMemoryBarrier2()
			.setSrcStageMask(src_stage_mask)
			.setDstStageMask(dst_stage_mask)
			.setSrcAccessMask(src_stage_access)
			.setDstAccessMask(dst_stage_access)
			.setOldLayout(old_layout)
			.setNewLayout(new_layout)
			.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setImage(image)
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(aspectMask)
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1));

		std::array<vk::ImageMemoryBarrier2, 1> barriers = { barrier };
		vk::DependencyInfo dependencyInfo = vk::DependencyInfo()
			.setDependencyFlags(vk::DependencyFlagBits::eByRegion)
			.setImageMemoryBarriers(barriers);

		cmdBuffer.pipelineBarrier2(dependencyInfo);
		
	}

	void Window::createDepthResources(Device& device)
	{
		// Createing Depth Image
		vk::ImageCreateInfo imageCreateInfo = vk::ImageCreateInfo()
			.setImageType(vk::ImageType::e2D)
			.setFormat(depthFormat)
			.setExtent(vk::Extent3D(swapChainExtent.width, swapChainExtent.height, 1))
			.setMipLevels(1)
			.setArrayLayers(1)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setTiling(vk::ImageTiling::eOptimal)
			.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setInitialLayout(vk::ImageLayout::eUndefined);

		depthImage = vk::raii::Image(device.logicalDevice, imageCreateInfo);

		auto memRequirements = depthImage.getMemoryRequirements();
		
		// Binding and allocating memory for depth image
		auto memRequirement = depthImage.getMemoryRequirements();
		uint32_t memoryTypeIndex = device.findMemoryType(device.physicalDevice, memRequirement.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

		vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
			.setAllocationSize(memRequirement.size)
			.setMemoryTypeIndex(memoryTypeIndex);

		depthImageMemory = vk::raii::DeviceMemory(device.logicalDevice, allocInfo);
		depthImage.bindMemory(*depthImageMemory, 0);

		// Creating Depth Image View
		vk::ImageViewCreateInfo imageViewCreateInfo = vk::ImageViewCreateInfo()
			.setImage(depthImage)
			.setViewType(vk::ImageViewType::e2D)
			.setFormat(depthFormat)
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eDepth)
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1));
	
		depthImageView = vk::raii::ImageView(device.logicalDevice, imageViewCreateInfo);


		vk::ImageViewCreateInfo sampleViewCreateInfo = vk::ImageViewCreateInfo()
			.setImage(depthImage)
			.setViewType(vk::ImageViewType::e2D)
			.setFormat(depthFormat)
			.setSubresourceRange(vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eDepth)
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1))
			.setComponents(vk::ComponentMapping(
			vk::ComponentSwizzle::eR, 
			vk::ComponentSwizzle::eR, 
			vk::ComponentSwizzle::eR, 
			vk::ComponentSwizzle::eOne
		));;
	
		depthSampleView = vk::raii::ImageView(device.logicalDevice, sampleViewCreateInfo);
	}
}