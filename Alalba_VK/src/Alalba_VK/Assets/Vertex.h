#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <vulkan/vulkan.h>
/* reference*/
// https://github.com/BoomingTech/Piccolo/blob/main/engine/source/runtime/function/render/render_mesh.h

namespace Alalba
{

  struct MeshVertex {
    struct VertexPostition
    {
      glm::vec3 position;
      glm::vec3 color;
      glm::vec2 uv;
    };

    static std::array<VkVertexInputBindingDescription, 1> GetBindingDescriptions()
    {
      std::array<VkVertexInputBindingDescription, 1> binding_descriptions{};

      // position
      binding_descriptions[0].binding = 0;
      binding_descriptions[0].stride = sizeof(VertexPostition);
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
      attribute_descriptions[0].offset = offsetof(VertexPostition, position);

      // varying blending
      attribute_descriptions[1].binding = 0;
      attribute_descriptions[1].location = 1;
      attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
      attribute_descriptions[1].offset = offsetof(VertexPostition, color);

      attribute_descriptions[2].binding = 0;
      attribute_descriptions[2].location = 2;
      attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
      attribute_descriptions[2].offset = offsetof(VertexPostition, uv);

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