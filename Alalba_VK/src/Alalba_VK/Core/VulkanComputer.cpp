#include "pch.h"
#include "VulkanComputer.h"
#include "Alalba_VK/Vulkan/Device.h"
#include "Alalba_VK/Core/Application.h"

namespace vk
{
	void VulkanComputer::Init(const std::string& computshader)
	{
		
		m_cmdPool4Compute = CommandPool::Builder(m_device)
			.SetTag("Compute CmdPool")
			.SetFlags(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			.SetQFamily(m_device.GetComputeQ().GetFamily())
			.Build();

		m_cmdBuffers = CommandBuffers::Allocator(m_device, *m_cmdPool4Compute.get())
			.SetTag("CmdBuffers fot Computing")
			.OneTimeSubmit(true)
			.SetSize(1) 
			.Allocate();
		
		m_computeShaderModule = ShaderModule::Builder(m_device)
			// path relative to Sandbox
			.SetTag("Compute Shader")
			.SelectSpvFile(computshader)
			.SetShaderStageBits(VK_SHADER_STAGE_COMPUTE_BIT)
			.Build();

		// TODO: set descriptor layout and pipeline layout 
		
		// 1. descriptor set layout
		m_descSetLayout = DescriptorSetLayout::Builder(m_device)
			.SetTag(" Compute Desc set layout")
			.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
			.Build();
		// 2. pipeline layout
		m_pipelineLayout = PipelineLayout::Builder(m_device)
			.SetTag("Compute pipelline layout")
			.BindDescriptorSetLayout(*m_descSetLayout.get())
			.Build();
		// 3 .pipeline
		m_computePipeline = ComputePipeline::Builder(m_device, *m_pipelineLayout.get(), *m_computeShaderModule.get())
			.SetTag("Compute Pipeline")
			.Build();

		// 4. descriptor pool
		m_descPool = DescriptorPool::Builder(m_device)
			.SetTag("Compute descriptor pool")
			.SetMaxSets(1)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,1)
			.Build();
		// 5. descriptor sets
		m_descSets.push_back(
			DescriptorSet::Allocator(m_device, *m_descPool.get())
			.SetTag("computer Descritor Set ")
			.SetDescSetLayout(*m_descSetLayout.get())
			.Allocate()
		);
		// 6. set up descrptor set
		m_targetTexture.reset(new Alalba::Texture(1024, 1024, VK_FORMAT_R8G8B8A8_UNORM));
		m_descSets[0]->
			BindDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0,
				m_targetTexture->GetSampler(), m_targetTexture->GetImageView(), m_targetTexture->GetImage().Layout())
			.UpdateDescriptors();
		

		/// Synchronization
		m_fence = Fence::Builder(m_device)
			.SetTag("Computer Fence")
			.Signaled(true)
			.Build();
	}

	///https://github.com/nvpro-samples/vk_mini_path_tracer/blob/main/checkpoints/6_compute_shader/main.cpp

	void VulkanComputer::Execute()
	{
		m_fence->Wait(UINT64_MAX);
		m_fence->Reset();
		vkResetCommandBuffer((*m_cmdBuffers.get())[0], 0);

///Encode part
		CommandBuffers& cmdBuffers = (*m_cmdBuffers.get());
		
		std::vector<VkDescriptorSet>DescSets;
		// Important: the order of pushing back determine the set==xx in shader
		DescSets.push_back(m_descSets[0]->Handle());

		cmdBuffers.BeginRecording(0);
		{

			// Flush the queue if we're rebuilding the command buffer after a 
			// pipeline change to ensure it's not currently in use
			// Wait for the GPU to finish
			vkQueueWaitIdle(m_device.GetComputeQ().Handle());

			vkCmdBindPipeline(cmdBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipeline->Handle());
			vkCmdBindDescriptorSets(cmdBuffers[0],
				VK_PIPELINE_BIND_POINT_COMPUTE, m_pipelineLayout->Handle(), 0, DescSets.size(), DescSets.data(), 0, nullptr);
			// Run the compute shader with one workgroup for now
			vkCmdDispatch(cmdBuffers[0], 1024/16, 1026/8, 1);
		}
		cmdBuffers.EndRecording(0);

		/// submit
		
		// Submit the command buffer
		VkCommandBuffer commandBuffers[]{ (*m_cmdBuffers.get())[0] };
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = commandBuffers;
		// submitInfo.waitSemaphoreCount = 1;
		// submitInfo.pWaitSemaphores = &graphics.semaphore;
		// submitInfo.pWaitDstStageMask = &waitStageMask;
		// submitInfo.signalSemaphoreCount = 1;
		// submitInfo.pSignalSemaphores = &compute.semaphore;

		VkResult err;
		err = vkQueueSubmit(m_device.GetComputeQ().Handle(),
			1, &submitInfo, m_fence->Handle());
		ALALBA_ASSERT(err == VK_SUCCESS, "Compute Q submit failed");
	}

	void VulkanComputer::Shutdown()
	{
		m_device.WaitIdle();
		
		m_targetTexture->Clean();

		m_pipelineLayout->Clean();
		m_computeShaderModule->Clean();
		m_computePipeline->Clean();
		
		m_cmdBuffers->Clean();
		m_cmdPool4Compute->Clean();

		m_descSetLayout->Clean();
		m_descSetLayout->Clean();

		m_descPool->Clean();

		m_fence->Clean();
	}

}