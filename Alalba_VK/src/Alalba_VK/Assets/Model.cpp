#include "pch.h"
#include "Model.h"
#include "Alalba_VK/Vulkan/Buffer.h"
#include "Alalba_VK/Core/Application.h"
#include "Alalba_VK/Vulkan/Allocator.h"
namespace Alalba
{
	vk::Allocator* Mesh::s_allocator = nullptr;
	//vk::Allocator* Mesh::s_allocator = new vk::Allocator(Application::Get().GetDevice(), Alalba::Application::Get().GetVulkanInstance(), "Mesh Allocator");
	Mesh::Mesh()
	{
		if (s_allocator == nullptr)
			s_allocator = new vk::Allocator(Application::Get().GetDevice(), Alalba::Application::Get().GetVulkanInstance(), "Mesh Allocator");

		uint32_t vertexSize = sizeof(vertices[0]) * vertices.size();
		uint32_t indexSize = sizeof(indices[0]) * indices.size();
	
		// Vertex buffer
		// should has its own allocator
		m_vertexBuffer = vk::Buffer::Builder(Application::Get().GetDevice(), *s_allocator)
			.SetTag("Vertex Buffer")
			.SetSize(vertexSize)
			.SetUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
			.SetVmaUsage(VMA_MEMORY_USAGE_GPU_ONLY)
			.Build();
		
		/* 
			https://vulkan-tutorial.com/Vertex_buffers/Staging_buffer#page_Transfer-queue
			Use graphic queue, since both compute and graphics q support transfer
			m_copyCmdBuffer->Flush(0, Application::Get().GetDevice().GetTransferQ());
		*/
		m_vertexBuffer->CopyDataFrom(
			vertices.data(), vertexSize,
			Application::Get().GetDevice().GetGraphicsQ(), Application::Get().GetRenderer().GetCommandPool()
		);

		// Index buffer
		m_indexBuffer = vk::Buffer::Builder(Application::Get().GetDevice(), *s_allocator)
			.SetTag("Index Buffer")
			.SetSize(indexSize)
			.SetUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
			.SetVmaUsage(VMA_MEMORY_USAGE_GPU_ONLY)
			.Build();
		
		m_indexBuffer->CopyDataFrom(
			indices.data(), indexSize,
			Application::Get().GetDevice().GetGraphicsQ(), Application::Get().GetRenderer().GetCommandPool()
		);
	}

	void Mesh::Clean()
	{
		m_vertexBuffer->Clean();
		m_indexBuffer->Clean();
	}
}