#pragma once
#include "BasicRenderSys.h"
#include "DiffractionSys.h"

#include "Alalba_VK/Vulkan/SwapChain.h"
#include "Alalba_VK/Vulkan/ShaderModule.h"
#include "Alalba_VK/Vulkan/Image.h"
#include "Alalba_VK/Vulkan/ImageView.h"

#include "Alalba_VK/Vulkan/FrameBuffer.h"
#include "Alalba_VK/Vulkan/RenderPass.h"
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
namespace Alalba
{
	class Renderer
	{
	public:
		Renderer(Scene& scene);
		~Renderer() {};

		void Shutdown();
		void Resize();
	
		
		void PrepareCommandBuffer(Scene& scene);
		void UpdateUBO(Scene& scene);
		void DrawFrame(Scene& scene);
	private:
	
	private:
		
		struct GlobalUBO {
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
			glm::vec3 position;
		};

	private:
		Scene& m_scene;
		// 
		std::unique_ptr<vk::Allocator> m_allocator;

		// pipeline 
		std::unique_ptr<vk::PipelineCache> m_pipelineCache;
		

		// shader related Global 
		std::unique_ptr<vk::DescriptorPool> m_globalDescPool;
		std::vector<std::unique_ptr<vk::DescriptorSet>> m_GlobalDescriptorSets{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
		std::vector< std::unique_ptr<vk::Buffer> > m_globalUniformbuffers{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };

		// swapchain and renderpass 
		std::unique_ptr<vk::SwapChain> m_swapChain;
		std::unique_ptr<vk::RenderPass> m_renderPass;
		std::vector<std::unique_ptr<vk::FrameBuffer>> m_frameBuffers{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
		std::unique_ptr<vk::Image> m_depthImage;
		std::unique_ptr<vk::ImageView> m_depthImageView;
		
		// command
		std::unique_ptr<vk::CommandPool> m_commandPool;
		std::unique_ptr<vk::CommandBuffers> m_commandBuffers;

		// syschronize
		std::vector< std::unique_ptr<vk::Fence> > m_inFlightFences{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
		std::vector< std::unique_ptr<vk::Semaphore> > m_imageAvailableSemaphores{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
		std::vector< std::unique_ptr<vk::Semaphore> > m_renderFinishedSemaphores{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };

		/// Systems
		std::unique_ptr < vk::DescriptorSetLayout > m_globalDescSetLayout;
		// basic sys
		bool m_BasicSysOn = false;
		std::unique_ptr<BasicRenderSys> m_basicRenderSys;
		std::unique_ptr < vk::DescriptorSetLayout > m_basicDescSetLayout;
		// diffraction sys
		bool m_DiffractionSysOn = true;
		std::unique_ptr<DiffractionSys> m_diffractionRenderSys;
		std::unique_ptr < vk::DescriptorSetLayout > m_diffractionDescSetLayout;
		// other sys


		uint32_t m_currentFrame = 0;
		GlobalUBO ubo;

	};
}


