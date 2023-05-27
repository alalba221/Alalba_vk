#include "pch.h"
#include "Mesh.h"
#include "Alalba_VK/Vulkan/Buffer.h"
#include "Alalba_VK/Core/Application.h"

#include "MeshSys.h"

namespace Alalba
{
	Mesh::Mesh(MeshSys& sys, const std::string& file)
	{
		// 
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		LoadModel(file, vertices,indices);

		uint32_t vertexSize = sizeof(Vertex) * vertices.size();
		uint32_t indexSize = sizeof(uint32_t) * indices.size();
		
		m_indexCount = indices.size();

		// Vertex buffer
		m_vertexBuffer = vk::Buffer::Builder(Application::Get().GetDevice(), sys.Allocator())
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
			Application::Get().GetDevice().GetGraphicsQ(), sys.CmdPool()
		);

		// Index buffer
		m_indexBuffer = vk::Buffer::Builder(Application::Get().GetDevice(), sys.Allocator())
			.SetTag("Index Buffer")
			.SetSize(indexSize)
			.SetUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
			.SetVmaUsage(VMA_MEMORY_USAGE_GPU_ONLY)
			.Build();
		
		m_indexBuffer->CopyDataFrom(
			indices.data(), indexSize,
			Application::Get().GetDevice().GetGraphicsQ(), sys.CmdPool()
		);

		vertices.clear();
		indices.clear();
	}

	void Mesh::Clean()
	{
		//vkDeviceWaitIdle(Application::Get().GetDevice().Handle());
		m_vertexBuffer->Clean();
		m_indexBuffer->Clean();
	}
	void Mesh::LoadModel(const std::string& file, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
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

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes) 
		{
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};
				
				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
				
				vertex.uv = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}

	}
}