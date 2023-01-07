#include "pch.h"
#include "VulkanComputer.h"
#include "Alalba_VK/Vulkan/Device.h"
#include "Alalba_VK/Vulkan/Allocator.h"
#include "Alalba_VK/Core/Application.h"
namespace vk
{
	void VulkanComputer::Init()
	{
		
		m_cmdPool4Compute = CommandPool::Builder(m_device)
			.SetTag("Compute CmdPool")
			.SetFlags(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			.SetQFamily(Alalba::Application::Get().GetVulkanInstance().GetPhysicalDevice().GetQFamilies().compute.value())
			.Build();

		m_cmdBuffers = CommandBuffers::Allocator(m_device, *m_cmdPool4Compute.get())
			.SetTag("CmdBuffers fot Computing")
			.OneTimeSubmit(true)
			.SetSize(1) 
			.Allocate();
		
		m_computeShaderModule = ShaderModule::Builder(m_device)
			// path relative to Sandbox
			.SetTag("Compute Shader")
			.SelectSpvFile("../Alalba_VK/src/Alalba_VK/Shaders/comp.spv")
			.SetShaderStageBits(VK_SHADER_STAGE_COMPUTE_BIT)
			.Build();

		// TODO: set descriptor layout and pipeline layout 
		m_pipelineLayout = PipelineLayout::Builder(m_device)
			.SetTag("Compute pipelline layout")
			.Build();
		// 
		m_computePipeline = ComputePipeline::Builder(m_device, *m_pipelineLayout.get(), *m_computeShaderModule.get())
			.SetTag("Compute Pipeline")
			.Build();
	}

	///https://github.com/nvpro-samples/vk_mini_path_tracer/blob/main/checkpoints/6_compute_shader/main.cpp

	void VulkanComputer::Execute()
	{
		CommandBuffers& cmdBuffers = (*m_cmdBuffers.get());
		
		cmdBuffers.BeginRecording(0);
		{
			vkCmdBindPipeline(cmdBuffers[0], VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipeline->Handle());
			// Run the compute shader with one workgroup for now
			vkCmdDispatch(cmdBuffers[0], 1, 1, 1);

			// Add a command that says "Make it so that memory writes by the compute shader
			// are available to read from the CPU." (In other words, "Flush the GPU caches
			// so the CPU can read the data.") To do this, we use a memory barrier.
			// This is one of the most complex parts of Vulkan, so don't worry if this is
			// confusing! We'll talk about pipeline barriers more in the extras.
			VkMemoryBarrier memoryBarrier{};
			memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;  // Make shader writes
			memoryBarrier.dstAccessMask = VK_ACCESS_HOST_READ_BIT;     // Readable by the CPU
			vkCmdPipelineBarrier(cmdBuffers[0],                              // The command buffer
				VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,   // From the compute shader
				VK_PIPELINE_STAGE_HOST_BIT,             // To the CPU
				0,                                      // No special flags
				1, &memoryBarrier,                      // An array of memory barriers
				0, nullptr, 0, nullptr);                // No other barriers
		}
		cmdBuffers.EndRecording(0);

		/// submit
		
		// Submit the command buffer
		VkCommandBuffer commandBuffers[]{ (*m_cmdBuffers.get())[0] };
		VkSubmitInfo submitInfo = {};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = commandBuffers;
		VkResult err;
		err = vkQueueSubmit(m_device.GetComputeQ().Handle(),
			1, &submitInfo, VK_NULL_HANDLE);
		ALALBA_ASSERT(err == VK_SUCCESS, "Compute Q submit failed");

		// Wait for the GPU to finish
		vkQueueWaitIdle(m_device.GetComputeQ().Handle());
	}

	void VulkanComputer::Shutdown()
	{
		m_device.WaitIdle();
		
		m_pipelineLayout->Clean();
		m_computeShaderModule->Clean();
		m_computePipeline->Clean();
		
		m_cmdBuffers->Clean();
		m_cmdPool4Compute->Clean();
	}

}