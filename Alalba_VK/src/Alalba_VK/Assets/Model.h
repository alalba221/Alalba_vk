#pragma once

#include "Vertex.h"
#include "tiny_obj_loader.h"

namespace vk
{
  class Buffer;
  class Allocator;
  class CommandPool;
}

namespace Alalba
{
  class Mesh
  {
  public:

    Mesh(const std::string file);
    void Clean();
    const vk::Buffer& GetVertexbuffer() const { return *m_vertexBuffer.get();}
    const vk::Buffer& GetIndexbuffer() const { return *m_indexBuffer.get(); }

    const uint32_t GetIndexCount() const { return m_indices.size(); }
    const uint32_t GetInstanceCount() const { return 1; }

    static vk::Allocator* Allocator() { return s_allocator; }
    static vk::CommandPool* CommandPool() { return s_commandPool; }


  private:
    std::unique_ptr<vk::Buffer> m_vertexBuffer;
    std::unique_ptr<vk::Buffer> m_indexBuffer;

    std::vector<MeshVertex> m_vertices;
    std::vector<uint32_t> m_indices;

    void LoadModel(const std::string& file);

    // Allocator for Mesh
    static vk::Allocator* s_allocator;
    static vk::CommandPool* s_commandPool;
  };
}
