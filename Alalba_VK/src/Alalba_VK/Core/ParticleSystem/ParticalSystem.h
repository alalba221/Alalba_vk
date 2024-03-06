#pragma once
#include "Alalba_VK/Vulkan/VulkanHeaders.h"
#include <glm/glm.hpp>
namespace Alalba
{
  struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec4 color;

    static VkVertexInputBindingDescription getBindingDescription() {
      VkVertexInputBindingDescription bindingDescription{};
      bindingDescription.binding = 0;
      bindingDescription.stride = sizeof(Particle);
      bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
      std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

      attributeDescriptions[0].binding = 0;
      attributeDescriptions[0].location = 0;
      attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
      attributeDescriptions[0].offset = offsetof(Particle, position);

      attributeDescriptions[1].binding = 0;
      attributeDescriptions[1].location = 1;
      attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
      attributeDescriptions[1].offset = offsetof(Particle, color);

      return attributeDescriptions;
    }
  };

	class ParticalSystem
	{
	public:
		ParticalSystem();
		~ParticalSystem() {};

		void Update();
		void ShutDown();

	private:
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    const uint32_t PARTICLE_COUNT = 8192;

    void createShaderStorageBuffers();
  private:
		std::unique_ptr < vk::ShaderModule > m_computerShader;
		
		std::unique_ptr<vk::Allocator> m_allocator;

    std::unique_ptr<vk::CommandPool> m_commandPool;
    std::unique_ptr<vk::CommandBuffers> m_commandBuffers;
		std::vector<std::unique_ptr<vk::Buffer>> m_SSBOs{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
		
		std::unique_ptr<vk::ComputePipeline> m_computePipeline;
		std::unique_ptr<vk::PipelineLayout> m_pipelineLayout;

	};
}
