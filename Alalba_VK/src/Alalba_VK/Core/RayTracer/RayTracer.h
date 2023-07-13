#pragma once

#include "Alalba_VK/Vulkan/CommandPool.h"
#include "Alalba_VK/Vulkan/CommandBuffers.h"
#include "Alalba_VK/Vulkan/Fence.h"
#include "Alalba_VK/Vulkan/Semaphore.h"
#include "Alalba_VK/Vulkan/Buffer.h"
#include "Alalba_VK/Vulkan/PipelineCache.h"
#include "Alalba_VK/Vulkan/DescriptorSet.h"
#include "Alalba_VK/Vulkan/DescriptorSetLayout.h"
#include <glm/glm.hpp>
#include "Alalba_VK/Core/Scene/Scene.h"
#include "Alalba_VK/Vulkan/AccelerationStructure.h"
#include "Alalba_VK/Vulkan/Allocator.h"
namespace Alalba
{
	class RayTracer
	{
	public:
		RayTracer(Scene& scene);
		~RayTracer() {};

		void Shutdown();
		void Resize();


		void PrepareCommandBuffer(Scene& scene);
		void Update(Scene& scene);
		void DrawFrame(Scene& scene);
	private:

	private:

		struct GlobalUBO {
			//glm::mat4 model; 
			glm::mat4 view;
			glm::mat4 proj;
			glm::vec3 position;
		};

	private:
		Scene& m_scene;
		std::unique_ptr<vk::Allocator> m_allocator;

		std::unique_ptr<vk::CommandPool> m_commandPool;
		std::unique_ptr<vk::TLAS> m_TLAS;

	};
}

