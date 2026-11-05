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
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
		uint32_t imageIndex, 
		vk::ImageLayout old_layout, 
		vk::ImageLayout new_layout, 
		vk::AccessFlags2 src_access_mask, 
		vk::AccessFlags2 dst_access_mask, 
		vk::PipelineStageFlags2 src_stage_mask, 
		vk::PipelineStageFlags2 dst_stage_mask)
	{
		vk::ImageMemoryBarrier2 barrier = vk::ImageMemoryBarrier2()
			.setSrcStageMask(src_stage_mask)
			.setDstStageMask(dst_stage_mask)
			.setSrcAccessMask(src_access_mask)
			.setDstAccessMask(dst_access_mask)
			.setOldLayout(old_layout)
			.setNewLayout(new_layout)
			.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setImage(swapChainImages[imageIndex])
			.setSubresourceRange(vk::ImageSubresourceRange()
			.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseMipLevel(0)
			.setLevelCount(1)
			.setBaseArrayLayer(0)
			.setLayerCount(1));
		vk::DependencyInfo dependencyInfo = vk::DependencyInfo()
			.setDependencyFlags(vk::DependencyFlagBits::eByRegion)
			.setImageMemoryBarrierCount(1)
			.setImageMemoryBarriers(barrier)
			.setPImageMemoryBarriers(&barrier);

		cmdBuffer.pipelineBarrier2(dependencyInfo);
		
	}
}