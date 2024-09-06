#include "pch.h"
#include "ParticalSystem.h"
#include "Alalba_VK/Core/Application.h"
#include <random>
namespace Alalba
{
	ParticalSystem::ParticalSystem()
	{
		Application& app = Application::Get();
		m_allocator.reset(new vk::Allocator(app.GetDevice(), app.GetVulkanInstance(), "Partical System Allocator"));
		const vk::Device& device = Application::Get().GetDevice();
		
		m_commandPool = vk::CommandPool::Builder(device)
			.SetTag("Partical Sys CmdPool")
			//.SetFlags(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			.SetFlags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			.SetQFamily(device.GetComputeQ(0).GetFamily())
			.Build();

		m_commandBuffers = vk::CommandBuffers::Allocator(device, *m_commandPool)
			.SetTag("CmdBuffers4 compute partical")
			.OneTimeSubmit(true)
			.SetCount(vk::SwapChain::MAX_FRAMES_IN_FLIGHT) // one for each image in swapchain
			.Allocate();
		
		createUniformBuffers();
		createShaderStorageBuffers();

		m_computerShader = vk::ShaderModule::Builder(device)
			// path relative to Sandbox
			.SetTag("Particle Computer Shader")
			.SelectSpvFile("shaders/shader_compute.comp.spv")
			.SetShaderStageBits(VK_SHADER_STAGE_COMPUTE_BIT)
			.Build();
		

		m_computeDescSetLayout = vk::DescriptorSetLayout::Builder(device)
			.SetTag("Compute Descriptor SetLayout")
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
			.AddBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
			.AddBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
			.Build();

		m_descPool = vk::DescriptorPool::Builder(device)
			.SetTag("Compute Descriptor Pool")
			.SetMaxSets(vk::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, vk::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, vk::SwapChain::MAX_FRAMES_IN_FLIGHT * 2)
			.Build();

		createComputeDescriptorSets();

		m_computePipelineLayout = vk::PipelineLayout::Builder(device)
			.SetTag("compute pipeline layout")
			.BindDescriptorSetLayout(*m_computeDescSetLayout)
			.Build()
			;

		m_pipelineCache = vk::PipelineCache::Builder(device)
			.SetTag("Compute PipelineCache")
			.Build();

		m_computePipeline = vk::ComputePipeline::Builder(device,*m_computePipelineLayout,*m_computerShader,*m_pipelineCache)
			.SetTag("Compute pipeline")
			.Build();

		createSynchronizer();

	}
	void ParticalSystem::Update()
	{
	}
	void ParticalSystem::ShutDown()
	{
		m_commandBuffers->Clean();
		m_commandPool->Clean();
		
		for (auto& ubo : m_UBOs)
		{
			ubo->Clean();
		}

		for (auto& ssbo : m_SSBOs)
		{
			ssbo->Clean();
		}

		m_computerShader->Clean();
		m_computeDescSetLayout->Clean();
		
		for (auto& set : m_descSets)
		{
			set->Clean();
		}
		m_descPool->Clean();
		m_computePipelineLayout->Clean();
		m_computePipeline->Clean();
		m_pipelineCache->Clean();

		for (int i = 0; i < vk::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_computeFinishedSemaphores[i]->Clean();
			m_computeInflightFence[i]->Clean();
		}

		m_allocator->Clean();
	}

	// only for the sample right now
	void ParticalSystem::createShaderStorageBuffers()
	{
		// Initialize particles
		std::default_random_engine rndEngine((unsigned)time(nullptr));
		std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

		// Initial particle positions on a circle
		std::vector<Particle> particles(PARTICLE_COUNT);
		for (auto& particle : particles) {
			float r = 0.25f * sqrt(rndDist(rndEngine));
			float theta = rndDist(rndEngine) * 2.0f * 3.14159265358979323846f;
			float x = r * cos(theta) * HEIGHT / WIDTH;
			float y = r * sin(theta);
			particle.position = glm::vec2(x, y);
			particle.velocity = glm::normalize(glm::vec2(x, y)) * 0.00025f;
			particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
		}

		VkDeviceSize bufferSize = sizeof(Particle) * PARTICLE_COUNT;

		const vk::Device& device = Application::Get().GetDevice();

		for (int i = 0; i < vk::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_SSBOs[i] = vk::Buffer::Builder(device, *m_allocator)
				.SetTag("partical sys SSBO" + std::to_string(i))
				.SetUsage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)
				.SetVmaUsage(VMA_MEMORY_USAGE_GPU_ONLY)
				.SetSize(bufferSize)
				.Build();

			m_SSBOs[i]->CopyDataFrom(particles.data(), bufferSize, Application::Get().GetDevice().GetGraphicsQ(0), *m_commandPool);
		}
	}
	void ParticalSystem::createUniformBuffers()
	{
		const vk::Device& device = Application::Get().GetDevice();

		for (int i = 0; i < vk::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_UBOs[i] = vk::Buffer::Builder(device, *m_allocator)
				.SetTag("Compute Uniform Buffer" + std::to_string(i))
				.SetSize(sizeof(UniformBufferObject))
				.SetUsage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
				.SetVmaUsage(VMA_MEMORY_USAGE_CPU_TO_GPU)
				.Build();

			m_UBOs[i]->MapMemory();
		}
	}
	void ParticalSystem::createComputeDescriptorSets()
	{
		const vk::Device& device = Application::Get().GetDevice();
		for (int i = 0; i < vk::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_descSets[i] =
				vk::DescriptorSet::Allocator(device, *m_descPool)
				.SetTag("Compute Descritor Set " + std::to_string(i))
				.SetDescSetLayout(*m_computeDescSetLayout)
				.Allocate();

			int currentSSBO = i;
			int prevSSBO = (currentSSBO + vk::SwapChain::MAX_FRAMES_IN_FLIGHT - 1) % vk::SwapChain::MAX_FRAMES_IN_FLIGHT;
			
			m_descSets[i]->BindDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,0,*m_UBOs[i],0,sizeof(UniformBufferObject))
				.BindDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, *m_SSBOs[prevSSBO], 0, sizeof(Particle)* PARTICLE_COUNT)
				.BindDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2, *m_SSBOs[currentSSBO], 0, sizeof(Particle) * PARTICLE_COUNT)
				.UpdateDescriptors();
		}
	}
	void ParticalSystem::recordComputeCommandBuffer(uint32_t currentFrame)
	{
		vk::CommandBuffers& cmdBuffers = (*m_commandBuffers);

		cmdBuffers.BeginRecording(currentFrame);
		{
			vkCmdBindPipeline(cmdBuffers[currentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipeline->Handle());
			
			VkDescriptorSet dscset = m_descSets[currentFrame]->Handle();
			vkCmdBindDescriptorSets(cmdBuffers[currentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipelineLayout->Handle(), 
				0, 1, &dscset, 0, nullptr);

			vkCmdDispatch(cmdBuffers[currentFrame], PARTICLE_COUNT / 256, 1, 1);

		}
		cmdBuffers.EndRecording(currentFrame);
	}
	void ParticalSystem::createSynchronizer()
	{
		const vk::Device& device = Application::Get().GetDevice();

		for (int i = 0; i < vk::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_computeInflightFence[i] =
				vk::Fence::Builder(device)
				.SetTag("Compute InFlightFence " + std::to_string(i))
				.Signaled(true)
				.Build();

			m_computeFinishedSemaphores[i] = vk::Semaphore::Builder(device)
				.SetTag("Compute Finished Semaphore " + std::to_string(i))
				.Build();

		}
	}
}
