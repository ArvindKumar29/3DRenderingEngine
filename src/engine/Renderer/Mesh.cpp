#include "Mesh.h"

namespace VKRENDERER
{
	Model Mesh::loadFromFile(const std::string& filePath, Device& device, vk::raii::CommandBuffer& cmdBuffer, VmaAllocator allocator)
	{
		fastgltf::Expected<fastgltf::GltfDataBuffer> data = fastgltf::GltfDataBuffer::FromPath(filePath);
		if (data.error() != fastgltf::Error::None)
			throw std::runtime_error("Could not Load mesh Data!!!");
	
		fastgltf::Expected<fastgltf::Asset> asset = parser.loadGltf(data.get(), std::filesystem::path(filePath).parent_path(), fastgltf::Options::LoadExternalBuffers);
		if (asset.error() != fastgltf::Error::None)
			throw std::runtime_error("Could not load assets!!!");
	
		Model model;
	
		for (const auto& mesh : asset.get().meshes)
		{
			for (const auto& primitive : mesh.primitives)
			{
				Primitive prim;
				prim.firstIndex = static_cast<uint32_t>(allIndices.size());
				prim.vertexOffset = static_cast<uint32_t>(allVertices.size());

				if (primitive.indicesAccessor.has_value())
				{
					// loading Index Buffer data for the primitive
					const auto& indexAccessor = asset.get().accessors[primitive.indicesAccessor.value()];
					prim.indexCount = static_cast<uint32_t>(indexAccessor.count);
					fastgltf::iterateAccessor<uint32_t>(asset.get(), indexAccessor, [&](uint32_t idx) { allIndices.push_back(idx); });

					std::vector<Vertex> localVerts;
					// Loading Position Data
					const auto* posAttr = primitive.findAttribute("POSITION");
					if (posAttr)
					{
						const auto& posAccessor = asset.get().accessors[posAttr->accessorIndex];
						localVerts.resize(posAccessor.count);
						fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset.get(), posAccessor, [&](fastgltf::math::fvec3 pos, size_t i) { localVerts[i].pos = glm::vec3(pos.x(), pos.y(), pos.z()); });
					}

					// Loading Normal data
					const auto* normAttr = primitive.findAttribute("NORMAL");
					if (normAttr)
					{
						const auto& normAccessor = asset.get().accessors[normAttr->accessorIndex];
						fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset.get(), normAccessor, [&](fastgltf::math::fvec3 n, size_t i) { localVerts[i].normal = glm::vec3(n.x(), n.y(), n.z()); });
					}

					allVertices.insert(allVertices.end(), localVerts.begin(), localVerts.end());
				}
				model.primitives.push_back(prim);

			}
		}

		model.vb.createVertexBuffer(device, allVertices, cmdBuffer, allocator);
		model.ib.createindexBuffer(device, allIndices, cmdBuffer, allocator);
		return model;
	}
}