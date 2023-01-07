#pragma once
#include "Alalba_VK/Vulkan/ShaderModule.h"
#include "Alalba_VK/Vulkan/PipelineLayout.h"
#include "Alalba_VK/Vulkan/ComputePipeline.h" 

#include "Alalba_VK/Vulkan/CommandPool.h"
#include "Alalba_VK/Vulkan/CommandBuffers.h"
//#include "Alalba_VK/Vulkan/Fence.h"
//#include "Alalba_VK/Vulkan/Semaphore.h"
//#include "Alalba_VK/Vulkan/Buffer.h"
//
//#include "Alalba_VK/Vulkan/DescriptorSetLayout.h"
//#include "Alalba_VK/Vulkan/DescriptorPool.h"
//#include "Alalba_VK/Vulkan/DescriptorSet.h"


namespace vk
{
	class Device;
	class Allocator;

	class VulkanComputer
	{

	public:
		VulkanComputer(const VulkanComputer&) = delete;

		//inline static VulkanComputer& GetInstance(const Device& device, Allocator& allocator) 
		//{ 
		//	static VulkanComputer s_VulkanComputer(device, allocator); 
		//	return s_VulkanComputer; 
		//};

		void Shutdown();
		VulkanComputer(const Device& device) :m_device(device) {};
		void Init();
		void Execute();

		//const CommandPool& GetCommandPool() const { return *m_cmdPool4Compute.get(); }

	private:

		const class Device& m_device;

		std::unique_ptr<Allocator> m_allocator;

	
		std::unique_ptr<PipelineLayout> m_pipelineLayout;
		std::unique_ptr<ShaderModule> m_computeShaderModule;
		std::unique_ptr<ComputePipeline> m_computePipeline;
		std::unique_ptr<CommandPool> m_cmdPool4Compute;
		std::unique_ptr<CommandBuffers> m_cmdBuffers;

	};

}

