#pragma once
#include "Alalba_VK/Core/Application.h"

namespace vk
{
	class DescriptorPool;
	class RenderPass;
	class VulkanRenderer;
}
namespace Alalba
{
	class UI
	{
	public:
		UI(const vk::VulkanRenderer& renderer, const Window& window);
		~UI() {};

		void RenderCommand(const uint32_t frameBufferIndex) const;
		void Clean();
	private:
		std::unique_ptr<vk::DescriptorPool> m_descriptorPool;
		std::unique_ptr<vk::RenderPass> m_renderPass;
		
		const class vk::VulkanRenderer& m_renderer;
	};
}


