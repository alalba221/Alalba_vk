#pragma once
#include "Alalba_VK/Vulkan/Allocator.h"
#include "Alalba_VK/Vulkan/CommandPool.h"
#include "Mesh.h"

namespace Alalba 
{
	class MeshSys
	{
	public:
		MeshSys();
		~MeshSys() {};

		void Clean();

		vk::Allocator& Allocator() { return *m_allocator; }
		vk::CommandPool& CmdPool() { return *m_commandPool; }
	
		MeshSys& LoadMesh(const std::string& file);

		std::shared_ptr<Mesh> GetMesh(const std::string tag) //const 
		{
			return m_meshes[tag];
		}
	private:
		std::unique_ptr<vk::Allocator> m_allocator;
		std::unique_ptr<vk::CommandPool> m_commandPool;

		std::unordered_map<std::string, std::shared_ptr<Mesh> > m_meshes;
	};
}


