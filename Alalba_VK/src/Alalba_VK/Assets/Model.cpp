#include "pch.h"
#include "Model.h"

#include "Alalba_VK/Vulkan/CommandBuffers.h"
#include "Alalba_VK/Core/Application.h"
namespace Alalba
{
	Mesh::Mesh()
	{
		uint32_t vertexSize = sizeof(vertices[0]) * vertices.size();
		uint32_t indexSize = sizeof(indices[0]) * indices.size();

		// Vertex buffer
		m_vertexBuffer = vk::Buffer::Builder(Application::Get().GetDevice(), Application::Get().GetAllocator())
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
		Application::Get().GetAllocator().CopyDataToGPU(
			vertices.data(), *m_vertexBuffer.get(), vertexSize,
			Application::Get().GetDevice().GetGraphicsQ(), Application::Get().GetRenderer().GetCommandPool()
		);

		// Index buffer
		m_indexBuffer = vk::Buffer::Builder(Application::Get().GetDevice(), Application::Get().GetAllocator())
			.SetTag("Index Buffer")
			.SetSize(indexSize)
			.SetUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
			.SetVmaUsage(VMA_MEMORY_USAGE_GPU_ONLY)
			.Build();
		Application::Get().GetAllocator().CopyDataToGPU(
			indices.data(), *m_indexBuffer.get(), indexSize,
			Application::Get().GetDevice().GetGraphicsQ(), Application::Get().GetRenderer().GetCommandPool()
		);

	}

	void Mesh::Clean()
	{
		m_vertexBuffer->Clean();
		m_indexBuffer->Clean();
	}
}