#include "pch.h"
#include "RayTracer.h"
#include "Alalba_VK/Core/Application.h"
#include "Alalba_VK/Core/Scene/Entity.h"

namespace Alalba
{
	RayTracer::RayTracer(Scene& scene)
		:m_scene(scene)
	{

		Application& app = Application::Get();
		const vk::Device& device = app.GetDevice();


		m_allocator.reset(new vk::Allocator(device, app.GetVulkanInstance(), "RayTracer Allocator"));
		m_commandPool = vk::CommandPool::Builder(device)
			.SetTag("RayTracer CmdPool")
			//	.SetFlags(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			.SetFlags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			.SetQFamily(device.GetGraphicsQ().GetFamily())
			.Build();


		auto view = m_scene.GetAllEntitiesWith<TransformComponent,MeshComponent>();

		/// for each entity

		vk::TLAS::Builder tlasBuilder(device, *m_allocator);
		tlasBuilder.SetTag("TLAS");

		for (auto e : view)
		{
			Entity entity{ e,&scene };

			auto& mesh = entity.GetComponent<MeshComponent>().m_Mesh;
			auto& transform = entity.GetComponent<TransformComponent>().Transform;
			
			std::string tag = entity.GetComponent<TagComponent>().Tag;
			ALALBA_INFO("Add instance for {0}", tag);
			tlasBuilder.AddInstance(mesh->GetBLAS(), transform, 0, 0);
		}
		m_TLAS = tlasBuilder.Build(device.GetGraphicsQ(), *m_commandPool);
	}
	void RayTracer::Shutdown()
	{
		const vk::Device& device = Application::Get().GetDevice();
		device.WaitIdle();


		m_TLAS->Clean();

		m_commandPool->Clean();
		m_allocator->Clean();
	}
}
