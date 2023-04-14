#pragma once

#include "Alalba_VK/Assets/ObjModel.h"
#include "Alalba_VK/Assets/Texture.h"
#include "Alalba_VK/Assets/Mesh.h"

#include "Alalba_VK/Vulkan/Allocator.h"
#include "Alalba_VK/Vulkan/CommandPool.h"

#include "Alalba_VK/Core/UniformBufferDeclaration.h"

namespace Alalba
{
	struct UniformBufferCPU
	{
		void* ptr;
		uint32_t size;
	};

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
		

		static inline vk::DescriptorSetLayout* GetGlobalDescLayout() { return s_globalDescSetLayout; }

		// TODO: move this transit function outof scene
		void UpdateGlobalUniform(const UniformBufferDeclaration& uniform);
		const UniformBufferCPU GetUniform()const { return uniform_cpu; };

		vk::Allocator& GetAllocator() const { return *m_allocator.get(); }
		const vk::CommandPool& GetCommandPool() const { return *m_cmdPool.get(); }

		vk::DescriptorSet& GetGlobalDescSet(uint32_t index)const { return *m_globalDescSets[index].get(); }
		vk::Buffer& GetGlobalUniformBuffer(uint32_t index)const { return *m_globalUniformbuffers[index].get(); }
		const vk::DescriptorPool& GetDescPool() const { return *m_globalDescPool.get(); }

	private:
		std::unique_ptr<vk::Allocator> m_allocator;
		std::unique_ptr<vk::CommandPool> m_cmdPool;

		std::unique_ptr<vk::DescriptorPool> m_globalDescPool;
		static vk::DescriptorSetLayout* s_globalDescSetLayout;
		std::vector<std::unique_ptr<vk::DescriptorSet>> m_globalDescSets;
		std::vector< std::unique_ptr<vk::Buffer> > m_globalUniformbuffers;
	
		std::unordered_map<std::string, std::unique_ptr<ObjModel> > m_models;
		std::unordered_map<std::string, std::unique_ptr<Texture> > m_textures;
		std::unordered_map<std::string, std::unique_ptr<Mesh> > m_meshes;

		UniformBufferCPU uniform_cpu;
	};
}
