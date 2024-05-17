//#pragma once
//
//
//#include "Alalba_VK/Vulkan/VulkanHeaders.h"
//#include <glm/glm.hpp>
//#include "Partical.h"
//
//namespace Alalba
//{
//	class ParticalRenderer
//	{
//	public:
//		ParticalRenderer();
//		~ParticalRenderer() {};
//
//		void Shutdown();
//		void Resize();
//		
//		const vk::RenderPass& GetRenderPass()const { return *m_renderPass; }
//		
//		void PrepareCommandBuffers();
//		void UpdateCommandBuffer(uint32_t currentFrame);
//
//		void Update();
//		void DrawFrame();
//
//	private:
//		
//		// 
//		vk::AllocatorPtr m_allocator;
//
//		// pipeline 
//		vk::PipelineCachePtr m_pipelineCache;
//		
//
//		// shader related Global 
//		vk::DescriptorPoolPtr m_globalDescPool;
//		std::vector<vk::DescriptorSetPtr> m_GlobalDescriptorSets{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
//		std::vector< vk::BufferPtr > m_globalUniformbuffers{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
//
//		// swapchain and renderpass 
//		vk::SwapChainPtr m_swapChain;
//		vk::RenderPassPtr m_renderPass;
//		std::vector<vk::FrameBufferPtr> m_frameBuffers{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
//		
//		std::vector< vk::ImagePtr > m_depthImages{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
//		std::vector< vk::ImageViewPtr > m_depthImageViews{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
//		
//		// command
//		vk::CommandPoolPtr m_commandPool;
//		vk::CommandBufferPtrs m_commandBuffers;
//
//		// syschronize
//		std::vector< vk::FencePtr > m_inFlightFences{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
//		
//		std::vector<vk::SemaphorePtr>  m_imageAvailableSemaphores{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
//		std::vector<vk::SemaphorePtr> m_renderFinishedSemaphores{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
//
//		/// Systems
//		vk::DescriptorSetLayoutPtr m_globalDescSetLayout;
//		
//		//// DebugSys sys
//		//bool m_DeugSysOn = true;
//		//std::unique_ptr<DebugSys> m_DebugSys;
//
//
//		uint32_t m_currentFrame = 0;
//
//	};
//}
//
//
