#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace VKRENDERER
{
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		static vk::VertexInputBindingDescription getBindingDescription()
		{
			vk::VertexInputBindingDescription inputBindingDescription = vk::VertexInputBindingDescription()
				.setBinding(0)
				.setStride(sizeof(Vertex))
				.setInputRate(vk::VertexInputRate::eVertex);
			return inputBindingDescription;
		}

		static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescription()
		{
			std::array<vk::VertexInputAttributeDescription, 3> inputAttributeDescription{};
			inputAttributeDescription[0] = vk::VertexInputAttributeDescription()
				.setBinding(0)
				.setLocation(0)
				.setFormat(vk::Format::eR32G32B32Sfloat)
				.setOffset(offsetof(Vertex, pos));
			inputAttributeDescription[1] = vk::VertexInputAttributeDescription()
				.setBinding(0)
				.setLocation(1)
				.setFormat(vk::Format::eR32G32B32Sfloat)
				.setOffset(offsetof(Vertex, color));
			inputAttributeDescription[2] = vk::VertexInputAttributeDescription()
				.setBinding(0)
				.setOffset(offsetof(Vertex, texCoord))
				.setLocation(2)
				.setFormat(vk::Format::eR32G32Sfloat);
			return inputAttributeDescription;
		}
	};
}