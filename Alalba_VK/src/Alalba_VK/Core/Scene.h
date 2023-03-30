#pragma once

#include "Alalba_VK/Assets/ObjModel.h"
#include "Alalba_VK/Assets/Texture.h"
#include "Alalba_VK/Assets/Mesh.h"

#include "Alalba_VK/Vulkan/Allocator.h"
#include "Alalba_VK/Vulkan/CommandPool.h"

namespace Alalba
{
	class Scene
	{
	public:
		Scene();
		~Scene() { Clean(); };
		void Clean();
		Scene& AddTexture(const std::string& file);
		Scene& AddMesh(const std::string& file);
		Scene& AddModel(std::string tag, std::string mesh, std::string texture);

		const std::unordered_map<std::string, std::unique_ptr<ObjModel> >& GetModels()const { return m_models; }
	private:
		std::unordered_map<std::string, std::unique_ptr<ObjModel> > m_models;
		std::unordered_map<std::string, std::unique_ptr<Texture> > m_textures;
		std::unordered_map<std::string, std::unique_ptr<Mesh> > m_meshes;

	public:
		static vk::Allocator* Allocator() { return s_allocator; }
		static vk::CommandPool* CommandPool() { return s_commandPool; }
	private:
		// Allocator for Mesh
		static vk::Allocator* s_allocator;
		static vk::CommandPool* s_commandPool;
	};
}
