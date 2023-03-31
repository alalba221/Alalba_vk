#include "pch.h"
#include "Scene.h"

#include "Alalba_VK/Core/Application.h"
namespace Alalba
{
	vk::Allocator* Scene::s_allocator = nullptr;
	vk::CommandPool* Scene::s_commandPool = nullptr;

	Scene::Scene()
	{
		// Static members
		if (s_allocator == nullptr)
			s_allocator = new vk::Allocator(Application::Get().GetDevice(), Alalba::Application::Get().GetVulkanInstance(), "Scene Allocator");
		if (s_commandPool == nullptr)
			s_commandPool = new vk::CommandPool(Application::Get().GetDevice(),
				Alalba::Application::Get().GetVulkanInstance().GetPhysicalDevice().GetQFamilies().graphics.value(),
				VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
				"Scene CommandPool");
	}
	void Scene::Clean()
	{
		for (auto& mesh : m_meshes)
			mesh.second->Clean();
		for (auto& texture : m_textures)
			texture.second->Clean();
		for (auto& mod : m_models)
			mod.second->Clean();

		if(s_allocator!= nullptr)
			s_allocator->Clean();
		if (s_commandPool != nullptr)
			s_commandPool->Clean();

		if (ObjModel::GetDescLayout() != nullptr)
			ObjModel::GetDescLayout()->Clean();
	}
	Scene& Scene::AddTexture(const std::string& file)
	{
		ALALBA_ASSERT(m_textures.count(file) == 0, "Texture already exist");
		m_textures.insert( std::make_pair(file,std::make_unique<Texture>(file)));
		return *this;
	}

	Scene& Scene::AddMesh(const std::string& file)
	{
		ALALBA_ASSERT(m_meshes.count(file) == 0, "Mesh already exist");
		//std::unique_ptr<Mesh> as = std::make_unique<Mesh>(file);
		m_meshes.insert( std::make_pair(file,std::make_unique<Mesh>(file)) );
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