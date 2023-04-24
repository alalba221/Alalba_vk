#pragma once
#include "Alalba_VK/Vulkan/Allocator.h"
#include "Alalba_VK/Vulkan/CommandPool.h"
#include "Alalba_VK/Assets/Texture.h"

#include "Alalba_VK/Vulkan/DescriptorPool.h"
#include "Alalba_VK/Vulkan/DescriptorSetLayout.h"
namespace Alalba
{
	class TextureSys
	{
	public:
		TextureSys();
		~TextureSys() {};

		void Clean();

		vk::Allocator& Allocator() { return *m_allocator.get(); }
		vk::CommandPool& CmdPool() { return *m_commandPool.get(); }

		TextureSys& LoadTexture(const std::string& file);

		std::shared_ptr<Texture> GetTexture(const std::string tag) //const 
		{
			return m_textures[tag];
		}
	private:
		std::unique_ptr<vk::Allocator> m_allocator;
		std::unique_ptr<vk::CommandPool> m_commandPool;
		//std::unique_ptr < vk::DescriptorPool > m_descriptorPool;
		//std::unique_ptr <vk::DescriptorSetLayout> m_descriptorSetLayout;

		std::unordered_map<std::string, std::shared_ptr<Texture> > m_textures;
	};
}


