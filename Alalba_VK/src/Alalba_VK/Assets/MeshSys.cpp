#include "pch.h"
#include "MeshSys.h"
#include "Alalba_VK/Core/Application.h"
namespace Alalba
{
	MeshSys::MeshSys()
	{
		Application& app  = Application::Get();
		m_allocator.reset(new vk::Allocator(app.GetDevice(), app.GetVulkanInstance(), "MeshSys Allocator"));
		m_commandPool = vk::CommandPool::Builder(app.GetDevice())
			.SetTag("MeshSys CmdPool")
			.SetFlags(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			.SetQFamily(app.GetDevice().GetGraphicsQ(0).GetFamily())
			.Build();
	}

	MeshSys& MeshSys::LoadMesh(const std::string& file)
	{
		ALALBA_ASSERT(m_meshes.count(file) == 0, "Mesh already exist");

		std::string tag = file.substr(file.rfind("/") + 1, file.rfind(".") - file.rfind("/") - 1);
		//m_meshes.insert(std::make_pair(tag, std::make_shared<Mesh>(*this, file)));
		m_meshes[tag].reset(new Mesh(*this, file));
		return *this;
	}

	void MeshSys::Clean()
	{
		for (auto& mesh : m_meshes)
		{
			mesh.second->Clean();
		}
		m_allocator->Clean();
		m_commandPool->Clean();
	}

}

