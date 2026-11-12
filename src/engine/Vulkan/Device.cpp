#include "Device.h"

namespace VKRENDERER
{
	Device::Device()
	{}

	Device::~Device()
	{}

	bool Device::isDeviceSuitable(vk::raii::PhysicalDevice& const pd, vk::raii::SurfaceKHR& surface, std::vector<char const*> requiredDeviceExtensions)
	{
		// Check support of vulkan 1.3 or above
		bool supportVulkan_1_3 = pd.getProperties().apiVersion >= VK_API_VERSION_1_3;
		bool isDGPU = pd.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu ? true : false;

		// Check queue family support for graphics and present
		auto queueFamilies = pd.getQueueFamilyProperties();
		uint32_t qfpIndex = 0;
		bool supportGraphicsAndPresent = std::ranges::any_of(queueFamilies,
			[&pd, &surface, &qfpIndex](auto const& qfp) {
				bool const suitable = (qfp.queueFlags & vk::QueueFlagBits::eGraphics) && (pd.getSurfaceSupportKHR(qfpIndex, *surface));
				qfpIndex++;
				return suitable;
			});
		
		// Check for all the physical Devices Extensions availability
		auto availableDeviceExtensions = pd.enumerateDeviceExtensionProperties();
		bool supportAllRequiredExtensions = std::ranges::all_of(requiredDeviceExtensions,
			[&availableDeviceExtensions](auto const& requiredDeviceExtensions) {
				return std::ranges::any_of(availableDeviceExtensions,
					[requiredDeviceExtensions](auto const& availableDeviceExtensions) {
						return strcmp(availableDeviceExtensions.extensionName, requiredDeviceExtensions) == 0; });
			});

		// Check for support of required features
		auto features = pd.template getFeatures2<vk::PhysicalDeviceFeatures2,
			vk::PhysicalDeviceVulkan11Features,
			vk::PhysicalDeviceVulkan13Features,
			vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
		
		bool supportRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
			features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
			features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;
		if (hasDGPU && !isDGPU)
			return false;
		else
			return supportVulkan_1_3 && supportGraphicsAndPresent && supportAllRequiredExtensions && supportRequiredFeatures;
	}

	void VKRENDERER::Device::getPhysicalDevice(vk::raii::Instance& instance, vk::raii::SurfaceKHR& surface, std::vector<char const*> requiredDeviceExtensions)
	{
		spdlog::info("============================GPU DETAILS================================");
		auto physicalDevices = instance.enumeratePhysicalDevices();
		for (auto pd : physicalDevices)
		{
			spdlog::info("GPU found:");
			spdlog::info("Device Name: {}", pd.getProperties().deviceName.data());
			spdlog::info("Device Type: {}", pd.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu);
			if (pd.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu){
				hasDGPU = true;
				break;
			}
		}

		auto const devIter = std::ranges::find_if(physicalDevices, [&](auto pd) { return isDeviceSuitable(pd, surface, requiredDeviceExtensions); });

		if (devIter == physicalDevices.end())
		{
			throw std::runtime_error("No GPUs with vulkan support found on system!!!");
		}

		physicalDevice = *devIter;
		
	}

	void Device::createLogicalDevice(std::vector<char const*> requiredDeviceExtensions)
	{
		std::vector<vk::QueueFamilyProperties> qfp = physicalDevice.getQueueFamilyProperties();

		auto graphicsQueueFamilyProperties = std::ranges::find_if(qfp, [](auto const& qfp) { return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0); });
		auto graphicsIndex = static_cast<uint32_t>(std::distance(qfp.begin(), graphicsQueueFamilyProperties));

		float queuePriority = 0.5f;
		vk::DeviceQueueCreateInfo deviceQueueCreateInfo = vk::DeviceQueueCreateInfo()
		.setQueueFamilyIndex(graphicsIndex)
		.setQueueCount(1)
		.setPQueuePriorities(&queuePriority);

		graphicsQueueIndex = graphicsIndex;

		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT vkExtFeatures = vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT()
			.setExtendedDynamicState(vk::True);
		vk::PhysicalDeviceVulkan13Features vk13Features = vk::PhysicalDeviceVulkan13Features()
			.setDynamicRendering(vk::True)
			.setSynchronization2(vk::True)
			.setPNext(&vkExtFeatures);
		vk::PhysicalDeviceVulkan11Features vk11Features = vk::PhysicalDeviceVulkan11Features()
			.setShaderDrawParameters(vk::True)
			.setPNext(&vk13Features);

		vk::DeviceCreateInfo deviceCreateInfo = vk::DeviceCreateInfo()
			.setPNext(&vk11Features)
			.setQueueCreateInfoCount(1)
			.setPQueueCreateInfos(&deviceQueueCreateInfo)
			.setEnabledExtensionCount(static_cast<uint32_t>(requiredDeviceExtensions.size()))
			.setPpEnabledExtensionNames(requiredDeviceExtensions.data());

		logicalDevice = vk::raii::Device(physicalDevice, deviceCreateInfo);
		graphicsQueue = vk::raii::Queue(logicalDevice, graphicsIndex, 0);
		
	}

	uint32_t Device::findMemoryType(vk::raii::PhysicalDevice& pd, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
	{
		auto memProperties = pd.getMemoryProperties();
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type!");
	}

}
