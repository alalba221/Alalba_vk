#pragma once

#include "Alalba_VK/Core/VulkanRenderer.h"
#include "Alalba_VK/Core/Window.h"
namespace Alalba
{
	class UI
	{
	public:
		UI(const vk::VulkanRenderer& renderer, const Window& window);
		~UI() {};

		void Draw();

	private:
		std::unique_ptr<vk::DescriptorPool> m_descriptorPool;

	};
}


