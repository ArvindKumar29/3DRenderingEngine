#include "Pipeline.h"


namespace VKRENDERER
{
	Pipeline::Pipeline() {}
	Pipeline::~Pipeline() {}

	vk::raii::ShaderModule Pipeline::createShaderModule(vk::raii::Device& ld, const std::vector<char>& code)
	{
		vk::ShaderModuleCreateInfo shaderModuleCreateInfo = vk::ShaderModuleCreateInfo()
			.setCodeSize(code.size())
			.setPCode(reinterpret_cast<const uint32_t*>(code.data()));
		return vk::raii::ShaderModule(ld, shaderModuleCreateInfo);
	}

	void Pipeline::createGraphicsPipeline(vk::raii::Device& ld, vk::Format surfaceFormat)
	{
		// reading spv files...
		auto vertShaderCode = readFile("shaders/main.vert.spv");
		auto fragShaderCode = readFile("shaders/main.frag.spv");

		// creating shader module...
		vk::raii::ShaderModule vertexShader = createShaderModule(ld, vertShaderCode);
		vk::raii::ShaderModule fragmentShader = createShaderModule(ld, fragShaderCode);

		// Shader Stage Create Infos...
		vk::PipelineShaderStageCreateInfo vertexShaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()
			.setStage(vk::ShaderStageFlagBits::eVertex)
			.setModule(*vertexShader)
			.setPName("main");
		vk::PipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = vk::PipelineShaderStageCreateInfo()
			.setStage(vk::ShaderStageFlagBits::eFragment)
			.setModule(*fragmentShader)
			.setPName("main");
		vk::PipelineShaderStageCreateInfo shaderStage[] = { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };

		// Vertex input for shaders(Vertex and Index buffers and other data) with appropriate memory allocation
		auto bindingDescription = Vertex::getBindingDescription();
		auto attributeDescription = Vertex::getAttributeDescription();
		vk::PipelineVertexInputStateCreateInfo vertexInputCreateInfo = vk::PipelineVertexInputStateCreateInfo()
			.setVertexBindingDescriptions(bindingDescription)
			.setVertexAttributeDescriptions(attributeDescription);


		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = vk::PipelineInputAssemblyStateCreateInfo()
			.setTopology(vk::PrimitiveTopology::eTriangleList)
			.setPrimitiveRestartEnable(vk::False);


		// Dynamic viewport and Scissors
		std::vector<vk::DynamicState> dynamicState = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
		vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo()
			.setDynamicStates(dynamicState);
		vk::PipelineViewportStateCreateInfo viewPortStateCreateInfo = vk::PipelineViewportStateCreateInfo()
			.setViewportCount(1)
			.setScissorCount(1);
		
		// Rasterizer 
		vk::PipelineRasterizationStateCreateInfo resterizerStateCreateInfo = vk::PipelineRasterizationStateCreateInfo()
			.setDepthClampEnable(vk::False)
			.setRasterizerDiscardEnable(vk::False)
			.setPolygonMode(vk::PolygonMode::eFill)
			.setLineWidth(1.0f)
			.setCullMode(vk::CullModeFlagBits::eNone)
			.setFrontFace(vk::FrontFace::eClockwise)
			.setDepthBiasEnable(vk::False);

		// MultiSampling
		vk::PipelineMultisampleStateCreateInfo multisampleCreateInfo = vk::PipelineMultisampleStateCreateInfo()
			.setSampleShadingEnable(vk::False)
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);

		// Color Blending 
		vk::PipelineColorBlendAttachmentState colorBlendAttachment = vk::PipelineColorBlendAttachmentState()
			.setColorWriteMask(
				vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
			)
			.setBlendEnable(vk::False);
		vk::PipelineColorBlendStateCreateInfo colorBlendCreateInfo = vk::PipelineColorBlendStateCreateInfo()
			.setLogicOpEnable(vk::False)
			.setAttachments(colorBlendAttachment);

		// Pipeline Layout
		vk::PushConstantRange pushConstantRange = vk::PushConstantRange()
			.setStageFlags(vk::ShaderStageFlagBits::eVertex)
			.setOffset(0)
			.setSize(sizeof(glm::mat4) * 3);

		vk::PipelineLayoutCreateInfo layoutCreateInfo = vk::PipelineLayoutCreateInfo()
			.setPushConstantRanges(pushConstantRange);
		graphicsPipeLineLayout = vk::raii::PipelineLayout(ld, layoutCreateInfo);

		// Dynamic Rendering
		vk::PipelineRenderingCreateInfo renderingCreateInfo = vk::PipelineRenderingCreateInfo()
			.setColorAttachmentFormats(surfaceFormat);
		// Final Graphics Pipeline Creation
		vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
			.setPNext(&renderingCreateInfo)
			.setStages(shaderStage)
			.setPVertexInputState(&vertexInputCreateInfo)
			.setPInputAssemblyState(&inputAssemblyCreateInfo)
			.setPViewportState(&viewPortStateCreateInfo)
			.setPRasterizationState(&resterizerStateCreateInfo)
			.setPMultisampleState(&multisampleCreateInfo)
			.setPColorBlendState(&colorBlendCreateInfo)
			.setPDynamicState(&dynamicStateCreateInfo)
			.setLayout(*graphicsPipeLineLayout);

		// Creating graphics Pipeline
		graphicsPipeline = vk::raii::Pipeline(ld, nullptr, graphicsPipelineCreateInfo);
	}

	std::vector<char> Pipeline::readFile(const std::string& fileName)
	{
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			spdlog::error("Shader file not found at {}", fileName);
			throw std::runtime_error("No shader file found!!!");
		}
		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), buffer.size());
		file.close();

		return buffer;
	}

	void Pipeline::createSyncObjects(vk::raii::Device& ld)
	{
		imageAvailableSemaphores = vk::raii::Semaphore(ld, vk::SemaphoreCreateInfo());
		renderFinishedSemaphore  = vk::raii::Semaphore(ld, vk::SemaphoreCreateInfo());
		renderFence = vk::raii::Fence(ld,
			vk::FenceCreateInfo()
			.setFlags(vk::FenceCreateFlagBits::eSignaled));
	}
}