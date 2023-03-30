#pragma once

#include "Geometry.h"

#include "Vertex.h"
#include "tiny_obj_loader.h"

// test
#include "glm/glm.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

namespace Alalba
{
  class Mesh:public Geometry
  {
  public:

    Mesh(const std::string& file);
    virtual void Clean() override;
    virtual const vk::Buffer& GetVertexbuffer() const override { return *m_vertexBuffer.get();}
    virtual const vk::Buffer& GetIndexbuffer() const override { return *m_indexBuffer.get(); }
    virtual const uint32_t GetIndexCount() const override { return m_indices.size(); }
    virtual const uint32_t GetInstanceCount() const override { return 1; }
    virtual const glm::mat4 ModelMatrix() const override { return m_model; }
    virtual Mesh& SetModelMatirx(glm::mat4 model) override { m_model = model; return *this; }
    ~Mesh() {};

  private:
    std::unique_ptr<vk::Buffer> m_vertexBuffer;
    std::unique_ptr<vk::Buffer> m_indexBuffer;
    std::vector<MeshVertex> m_vertices;
    std::vector<uint32_t> m_indices;

    void LoadModel(const std::string& file);

    glm::mat4 m_model{1.0f};


  };
}
