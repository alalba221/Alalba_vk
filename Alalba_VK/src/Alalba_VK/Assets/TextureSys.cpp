#include "pch.h"
#include "TextureSys.h"
#include "Alalba_VK/Core/Application.h"
namespace Alalba
{
	TextureSys::TextureSys()
	{
		Application& app = Application::Get();
		m_allocator.reset(new vk::Allocator(app.GetDevice(), app.GetVulkanInstance(), "TextureSys Allocator"));
		m_commandPool = vk::CommandPool::Builder(app.GetDevice())
			.SetTag("TextureSys CmdPool")
			.SetFlags(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			.SetQFamily(app.GetDevice().GetGraphicsQ().GetFamily())
			.Build();
	}

	TextureSys& TextureSys::LoadTexture(const std::string& file)
	{
		ALALBA_ASSERT(m_textures.count(file) == 0, "Texture already exist");

		std::string tag = file.substr(file.rfind("/") + 1, file.rfind(".") - file.rfind("/") - 1);
		m_textures.insert(std::make_pair(tag, std::make_shared<Texture>(VK_FORMAT_R8G8B8A8_SRGB, file, *m_allocator,*m_commandPool)));
		return *this;
	}

	void TextureSys::Clean()
	{
		for (auto& texture : m_textures)
		{
			texture.second->Clean();
		}
		m_allocator->Clean();
		m_commandPool->Clean();
	}
}