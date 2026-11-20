#pragma once
#include "Buffers.h"
#include "Vertex.h"
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/glm_element_traits.hpp>

namespace VKRENDERER
{
    struct Primitive {
        uint32_t firstIndex;
        uint32_t indexCount;
        int32_t  vertexOffset;
        glm::mat4 transform = glm::mat4(1.0f);
    };

    struct Model {
        vertexBuffer vb;
        indexBuffer ib;
        std::vector<Primitive> primitives;

        void destroy(VmaAllocator allocator) {
            vb.destroy(allocator);
            ib.destroy(allocator);
        }
    };

    class Mesh
    {
        std::vector<Vertex> allVertices;
        std::vector<uint32_t> allIndices;
        fastgltf::Parser parser;
    public:
        Model loadFromFile(const std::string& filePath, Device& device, vk::raii::CommandBuffer& cmdBuffer, VmaAllocator allocator);
    };
}