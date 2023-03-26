#pragma once
#include "Alalba_VK/Core/Application.h"

namespace vk
{
	class DescriptorPool;
	class RenderPass;
}
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
		std::unique_ptr<vk::RenderPass> m_renderPass;
	};
}


