#pragma once
#include "Alalba_VK/Core/GFX/GFXConstext.h"
#include "Instance.h"
#include "Surface.h"
namespace Alalba
{
	class Window;
}
namespace vk
{
	class GFXContext: public Alalba::GFXConstext
	{

	public:
		GFXContext(Alalba::Window* window);
		virtual ~GFXContext() override;

		const Instance& GetInstance() { return *m_vulkanInstance; }
		const Surface& GetSurface()  { return *m_vulkanSurface; }


	private:
		std::unique_ptr<vk::Instance> m_vulkanInstance;
		std::unique_ptr<vk::Surface> m_vulkanSurface;
		
	};
}
