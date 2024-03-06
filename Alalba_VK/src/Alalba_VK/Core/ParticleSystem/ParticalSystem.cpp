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
			.SetQFamily(device.GetComputeQ().GetFamily())
			.Build();
		m_commandBuffers = vk::CommandBuffers::Allocator(device, *m_commandPool)
			.SetTag("CmdBuffers4ParticalSystem")
			.OneTimeSubmit(true)
			.SetSize(vk::SwapChain::MAX_FRAMES_IN_FLIGHT) // one for each image in swapchain
			.Allocate();

		createShaderStorageBuffers();
		
	}
	void ParticalSystem::Update()
	{
	}
	void ParticalSystem::ShutDown()
	{
	}
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
		ALALBA_INFO(bufferSize);

		const vk::Device& device = Application::Get().GetDevice();

		for (int i = 0; i < vk::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_SSBOs[i] = vk::Buffer::Builder(device, *m_allocator)
				.SetTag("partical sys SSBO")
				.SetUsage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)
				.SetVmaUsage(VMA_MEMORY_USAGE_GPU_ONLY)
				.SetSize(bufferSize)
				.Build();

			m_SSBOs[i]->CopyDataFrom(particles.data(), bufferSize, Application::Get().GetDevice().GetComputeQ(), *m_commandPool);
		}
	}
}
