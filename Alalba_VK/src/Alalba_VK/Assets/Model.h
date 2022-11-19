#pragma once
#include "Alalba_VK/Vulkan/Buffer.h"
#include "Vertex.h"
 
namespace Alalba
{
  class Mesh
  {
  public:
    std::vector<MeshVertex::VertexPostition> vertices =
    {
        {{-0.5f,  -0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  -0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,   0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f,   0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}}
    };

    std::vector<uint32_t> indices = {
      0, 1, 2, 2, 3, 0
    };

    Mesh();
    void Clean();
    const vk::Buffer& GetVertexbuffer() const { return *m_vertexBuffer.get();}
    const vk::Buffer& GetIndexbuffer() const { return *m_indexBuffer.get(); }

    const uint32_t GetIndexCount() const { return indices.size(); }
    const uint32_t GetInstanceCount() const { return 1; }

  private:
    std::unique_ptr<vk::Buffer> m_vertexBuffer;
    std::unique_ptr<vk::Buffer> m_indexBuffer;
  };
}
