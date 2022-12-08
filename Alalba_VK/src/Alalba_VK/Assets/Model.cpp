#include "pch.h"
#include "Model.h"
#include "Alalba_VK/Vulkan/Buffer.h"
#include "Alalba_VK/Core/Application.h"
#include "Alalba_VK/Vulkan/Allocator.h"
#include "Alalba_VK/Vulkan/CommandPool.h"
namespace Alalba
{
	vk::Allocator* Mesh::s_allocator = nullptr;
	vk::CommandPool* Mesh::s_commandPool = nullptr;

	Mesh::Mesh(const std::string file)
	{
		// Static members
		if (s_allocator == nullptr)
			s_allocator = new vk::Allocator(Application::Get().GetDevice(), Alalba::Application::Get().GetVulkanInstance(), "Mesh Allocator");
		if (s_commandPool == nullptr)
			s_commandPool = new vk::CommandPool(Application::Get().GetDevice(),
				Alalba::Application::Get().GetVulkanInstance().GetPhysicalDevice().GetQFamilies().graphics.value(),
				VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
				"Model CommandPool");
		
		// 
		LoadModel(file);

		uint32_t vertexSize = sizeof(m_vertices[0]) * m_vertices.size();
		uint32_t indexSize = sizeof(m_indices[0]) * m_indices.size();
	
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
			m_vertices.data(), vertexSize,
			Application::Get().GetDevice().GetGraphicsQ(), *s_commandPool
		);

		// Index buffer
		m_indexBuffer = vk::Buffer::Builder(Application::Get().GetDevice(), *s_allocator)
			.SetTag("Index Buffer")
			.SetSize(indexSize)
			.SetUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
			.SetVmaUsage(VMA_MEMORY_USAGE_GPU_ONLY)
			.Build();
		
		m_indexBuffer->CopyDataFrom(
			m_indices.data(), indexSize,
			Application::Get().GetDevice().GetGraphicsQ(), *s_commandPool
		);
	}

	void Mesh::Clean()
	{
		//vkDeviceWaitIdle(Application::Get().GetDevice().Handle());
		m_vertexBuffer->Clean();
		m_indexBuffer->Clean();
	}
	void Mesh::LoadModel(const std::string& file)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file.c_str());
		if (!warn.empty()) {
			ALALBA_FATAL("WARN: {0}",warn);
		}
		ALALBA_ASSERT(err.empty(), err);

		std::unordered_map<MeshVertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes) 
		{
			for (const auto& index : shape.mesh.indices) {
				MeshVertex vertex{};

				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};
				vertex.color = { 1.0f, 1.0f, 1.0f };
				vertex.uv = 
				{
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(m_vertices.size());
					m_vertices.push_back(vertex);
				}
				m_indices.push_back(uniqueVertices[vertex]);
			}
		}

	}
}