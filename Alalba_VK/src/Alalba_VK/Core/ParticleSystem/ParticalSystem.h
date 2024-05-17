#pragma once
#include "Alalba_VK/Vulkan/VulkanHeaders.h"
#include "Partical.h"
namespace Alalba
{
  
	class ParticalSystem
	{
  public:
    

	public:
		ParticalSystem();
    ~ParticalSystem() { };

		void Update();
		void ShutDown();



	private:
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    const uint32_t PARTICLE_COUNT = 8192;

    /**
    * TODO
     * @brief only for the 2d circle now
     * 
    */
    void createShaderStorageBuffers();
    void createUniformBuffers();
    void createComputeDescriptorSets();
    void createSynchronizer();

    void recordComputeCommandBuffer(uint32_t currentFrame);
  private:

		vk::ShaderModulePtr  m_computerShader;
		
		vk::AllocatorPtr m_allocator;

    std::unique_ptr<vk::CommandPool> m_commandPool;
    vk::CommandBufferPtrs m_commandBuffers;

    std::vector<vk::BufferPtr> m_UBOs{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
		std::vector<vk::BufferPtr> m_SSBOs{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
		
  
    vk::DescriptorPoolPtr m_descPool;
    // 0. ubo; 1. prev frame ssbo, 2. current ssbo 
    vk::DescriptorSetLayoutPtr m_computeDescSetLayout;
    std::vector<vk::DescriptorSetPtr> m_descSets{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };

    vk::PipelineCachePtr m_pipelineCache;
    vk::PipelineLayoutPtr m_computePipelineLayout;
		vk::ComputePipelinePtr m_computePipeline;

    std::vector <vk::SemaphorePtr> m_computeFinishedSemaphores{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
    std::vector <vk::FencePtr> m_computeInflightFence{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
	};
}
