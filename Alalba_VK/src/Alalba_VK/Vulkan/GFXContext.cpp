#include "pch.h"
#include "GFXContext.h"
namespace vk
{
	GFXContext::GFXContext(Alalba::Window* window)
	{
		m_vulkanInstance = vk::Instance::Builder(window)
			.SetVulkanVersino(VK_API_VERSION_1_3)
#ifdef  ALALBA_DEBUG
			.RequestLayer("VK_LAYER_KHRONOS_validation")
#endif //  ALALBA_DEBUG
			.RequestLayer("VK_LAYER_LUNARG_monitor")

			.Build();

		m_vulkanSurface = Surface::Builder(*m_vulkanInstance, *window)
			.Build();
	}
	GFXContext::~GFXContext()
	{
		m_vulkanSurface->Clean();
		m_vulkanInstance->Clean();
	}
}
