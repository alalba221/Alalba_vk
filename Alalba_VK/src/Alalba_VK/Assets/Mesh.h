#pragma once

#include "Vertex.h"
#include "tiny_obj_loader.h"

// test
#include "glm/glm.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include "Alalba_VK/Vulkan/Buffer.h"
namespace Alalba
{
  class MeshSys;

  class Mesh
  {
  public:

    Mesh(MeshSys& sys,const std::string& file);
    
    void Clean();
    const vk::Buffer& GetVertexbuffer() const  { return *m_vertexBuffer;}
    const vk::Buffer& GetIndexbuffer() const { return *m_indexBuffer; }
    const uint32_t GetIndexCount() const { return m_indexCount; }
    const uint32_t GetInstanceCount() const  { return 1; }
    ~Mesh() {};

  private:
    std::unique_ptr<vk::Buffer> m_vertexBuffer;
    std::unique_ptr<vk::Buffer> m_indexBuffer;
    //std::vector<Vertex> m_vertices;
    //std::vector<uint32_t> m_indices;
    void LoadModel(const std::string& file, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    uint32_t m_indexCount;
  };
}
