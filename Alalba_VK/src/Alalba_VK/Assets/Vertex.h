#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <vulkan/vulkan.h>
/* reference*/
// https://github.com/BoomingTech/Piccolo/blob/main/engine/source/runtime/function/render/render_mesh.h

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
using namespace Alalba;


namespace Alalba
{

  struct MeshVertex {

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    bool operator==(const MeshVertex& other) const {
      return position == other.position && normal == other.normal && uv == other.uv;
    }

    static std::array<VkVertexInputBindingDescription, 1> GetBindingDescriptions()
    {
      std::array<VkVertexInputBindingDescription, 1> binding_descriptions{};

      // position
      binding_descriptions[0].binding = 0;
      binding_descriptions[0].stride = sizeof(MeshVertex);
      binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
      // varying blending
      //binding_descriptions[1].binding = 1;
      //binding_descriptions[1].stride = sizeof(VertexVaryingEnableBlending);
      //binding_descriptions[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
      //// varying
      //binding_descriptions[2].binding = 2;
      //binding_descriptions[2].stride = sizeof(VertexVarying);
      //binding_descriptions[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
      return binding_descriptions;
    }

    static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
    {
      std::array<VkVertexInputAttributeDescription, 3> attribute_descriptions{};

      // position
      attribute_descriptions[0].binding = 0;
      attribute_descriptions[0].location = 0;
      attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
      attribute_descriptions[0].offset = offsetof(MeshVertex, position);

      attribute_descriptions[1].binding = 0;
      attribute_descriptions[1].location = 1;
      attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
      attribute_descriptions[1].offset = offsetof(MeshVertex, normal);

      attribute_descriptions[2].binding = 0;
      attribute_descriptions[2].location = 2;
      attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
      attribute_descriptions[2].offset = offsetof(MeshVertex, uv);

      //attribute_descriptions[2].binding = 1;
      //attribute_descriptions[2].location = 2;
      //attribute_descriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
      //attribute_descriptions[2].offset = offsetof(VertexVaryingEnableBlending, tangent);

      //// varying
      //attribute_descriptions[3].binding = 2;
      //attribute_descriptions[3].location = 3;
      //attribute_descriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
      //attribute_descriptions[3].offset = offsetof(VertexVarying, texcoord);

      return attribute_descriptions;
    }
  };

}

namespace std {
  template<> struct hash<Alalba::MeshVertex> {
    size_t operator()(Alalba::MeshVertex const& vertex) const {
      return ((hash<glm::vec3>()(vertex.position) ^
        (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
        (hash<glm::vec2>()(vertex.uv) << 1);
    }
  };
}