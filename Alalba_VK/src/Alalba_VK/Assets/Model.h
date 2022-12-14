#pragma once

#include "Vertex.h"
#include "tiny_obj_loader.h"

// test
#include "glm/glm.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

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

    const glm::mat4 ModelMatrix() const{ return m_model; }
    Mesh& SetModelMatirx(glm::mat4 model) { m_model = model; return *this; }


    // testing usage 
    void test_UpdateModelMatrix()
    {
      static auto startTime = std::chrono::high_resolution_clock::now();
      auto currentTime = std::chrono::high_resolution_clock::now();
      float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
      m_model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }

  private:
    std::unique_ptr<vk::Buffer> m_vertexBuffer;
    std::unique_ptr<vk::Buffer> m_indexBuffer;

    std::vector<MeshVertex> m_vertices;
    std::vector<uint32_t> m_indices;

    void LoadModel(const std::string& file);

    // Allocator for Mesh
    static vk::Allocator* s_allocator;
    static vk::CommandPool* s_commandPool;

    glm::mat4 m_model{1.0f};


  };
}
