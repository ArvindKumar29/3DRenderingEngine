#include "CommandPool.h"
#include "..\Renderer\Camera.h"

namespace VKRENDERER
{
	CommandPool::CommandPool()
	{}

	CommandPool::~CommandPool()
	{}

	void CommandPool::createCommandPool(vk::raii::Device& ld, uint32_t queueIndex)
	{
		vk::CommandPoolCreateInfo poolInfo = vk::CommandPoolCreateInfo()
			.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
			.setQueueFamilyIndex(queueIndex);
		cmdPool = vk::raii::CommandPool(ld, poolInfo);
	}

	void CommandPool::creatCommandBuffer(vk::raii::Device& ld)
	{
		vk::CommandBufferAllocateInfo  allocInfo = vk::CommandBufferAllocateInfo()
			.setCommandPool(cmdPool)
			.setLevel(vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount(1);
		cmdBuffer = std::move(vk::raii::CommandBuffers(ld, allocInfo).front());
	}

	void CommandPool::recordCommandBuffer(vk::raii::Device& ld, vk::raii::PhysicalDevice& pd, vk::ImageView imageView, vk::ImageView depthImageView, vk::Extent2D extent, vk::raii::Pipeline& pipeline, vk::raii::PipelineLayout& pipelineLayout, vertexBuffer& vertexBuffer, indexBuffer& indexBuffer, glm::mat4 view, glm::mat4 proj, glm::mat4 model)
	{
		
		// Clear screen and set clear color
		vk::ClearValue clearColor;
		clearColor.color = vk::ClearColorValue(std::array<float, 4>{0.05f, 0.05f, 0.05f, 1.0f});

		// Setting up dynamic rendering
		vk::RenderingAttachmentInfo colorAttachment = vk::RenderingAttachmentInfo()
			.setImageView(imageView)
			.setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setClearValue(clearColor);

		vk::RenderingAttachmentInfo depthAttachment = vk::RenderingAttachmentInfo()
			.setImageView(depthImageView)
			.setImageLayout(vk::ImageLayout::eDepthAttachmentOptimal)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setClearValue(vk::ClearValue().setDepthStencil(vk::ClearDepthStencilValue(1.0f, 0)));

		vk::RenderingInfo renderingInfo = vk::RenderingInfo()
			.setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), extent))
			.setLayerCount(1)
			.setColorAttachments(colorAttachment)
			.setPDepthAttachment(&depthAttachment);

		cmdBuffer.beginRendering(renderingInfo);
		cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);
	
		// Setting viewport and Scissors
		vk::Viewport viewport(0.0f, 0.0f, static_cast<float>(extent.width), static_cast<float>(extent.height), 0.0f, 1.0f);
		cmdBuffer.setViewport(0, viewport);
		vk::Rect2D scissor(vk::Offset2D(0, 0), extent);
		cmdBuffer.setScissor(0, scissor);
		
		// Binding vertex buffer
		cmdBuffer.bindVertexBuffers(0, *vertexBuffer.buffer, { 0 });
		cmdBuffer.bindIndexBuffer(*indexBuffer.buffer, 0, vk::IndexType::eUint32);

		// Setting up push constants
		cmdBuffer.pushConstants(*pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), &view);
		cmdBuffer.pushConstants(*pipelineLayout, vk::ShaderStageFlagBits::eVertex, sizeof(glm::mat4), sizeof(glm::mat4), &proj);
		cmdBuffer.pushConstants(*pipelineLayout, vk::ShaderStageFlagBits::eVertex, sizeof(glm::mat4) * 2, sizeof(glm::mat4), &model);

		// Final draw command
		//cmdBuffer.draw(static_cast<uint32_t>(vertexBuffer.VB.size()), 1, 0, 0);
		cmdBuffer.drawIndexed(static_cast<uint32_t>(indexBuffer.IB.size()), 1, 0, 0, 0);

		cmdBuffer.endRendering();
		//cmdBuffer.end();

	}
}