#include "pch.h"
#include "Scene.h"

#include "Alalba_VK/Core/Application.h"
namespace Alalba
{

	Scene::Scene()
	{
		Application& app = Application::Get();
		m_allocator.reset(new vk::Allocator(app.GetDevice(), Alalba::Application::Get().GetVulkanInstance(), "Scene Allocator"));
		
		m_cmdPool = vk::CommandPool::Builder(app.GetDevice())
				.SetTag("Scene CmdPool")
				.SetFlags(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
				.SetQFamily(app.GetDevice().GetGraphicsQ().GetFamily())
				.Build();

	}
	void Scene::Clean()
	{
		for (auto& mesh : m_meshes)
			mesh.second->Clean();
		for (auto& texture : m_textures)
			texture.second->Clean();
		for (auto& mod : m_models)
			mod.second->Clean();

		m_allocator->Clean();
		m_cmdPool->Clean();

		if (ObjModel::GetDescLayout() != nullptr)
			ObjModel::GetDescLayout()->Clean();
	}
	Scene& Scene::AddTexture(const std::string& file)
	{
		ALALBA_ASSERT(m_textures.count(file) == 0, "Texture already exist");
		//m_textures.insert( std::make_pair(file,std::make_unique<Texture>(file)));
		m_textures.insert(std::make_pair(file, std::make_unique<Texture>(*this, file)));
		return *this;
	}

	Scene& Scene::AddMesh(const std::string& file)
	{
		ALALBA_ASSERT(m_meshes.count(file) == 0, "Mesh already exist");
		//std::unique_ptr<Mesh> as = std::make_unique<Mesh>(file);
		m_meshes.insert( std::make_pair(file,std::make_unique<Mesh>(*this,file)) );
		return *this;
	}

	Scene& Scene::AddModel(std::string tag, std::string mesh, std::string texture)
	{
		ALALBA_ASSERT(m_models.count(tag) == 0, "Model already exist");
		m_models.insert(std::make_pair(tag, std::make_unique<ObjModel>(*m_meshes[mesh].get(), *m_textures[texture].get())));
		return *this;
	}
	void Scene::UpdateGlobalUniform(const UniformBufferDeclaration& uniform)
	{
		uniform_cpu.ptr = (void*)(uniform.GetBuffer());
		uniform_cpu.size = uniform.GetBufferSize();
	}
}