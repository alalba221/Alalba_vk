#pragma once
#include "GLTFModel.h"
#include "Alalba_VK/Vulkan/Allocator.h"
#include "Alalba_VK/Vulkan/CommandPool.h"
#include "Alalba_VK/Vulkan/DescriptorPool.h"

namespace Alalba
{
	class GLTFLoader
	{
	public:
		GLTFLoader();
		~GLTFLoader() {};

		void Clean();

		vk::Allocator& Allocator() { return *m_allocator; }
		vk::CommandPool& CmdPool() { return *m_commandPool; }
		const vk::DescriptorPool& DescPool()const { return *m_descPool; }

		GLTFLoader& LoadModel(const std::string& file);

		std::shared_ptr<GLTFModel> GetModel(const std::string tag) //const 
		{
			return m_glTFmodels[tag];
		}

		const std::unordered_map<std::string, std::shared_ptr<GLTFModel> >& GetModelMap() const{ return m_glTFmodels; }

	private:
		std::unique_ptr<vk::Allocator> m_allocator;
		std::unique_ptr<vk::CommandPool> m_commandPool;
		std::unique_ptr<vk::DescriptorPool> m_descPool;

		std::unordered_map<std::string, std::shared_ptr<GLTFModel> > m_glTFmodels;
	};
}
