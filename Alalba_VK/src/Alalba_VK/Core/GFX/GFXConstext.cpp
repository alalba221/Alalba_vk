#include "pch.h"
#include "GFXConstext.h"
#include  "Alalba_VK/Vulkan/GFXContext.h"
namespace Alalba
{
	std::unique_ptr<GFXConstext> GFXConstext::Create(Window* window)
	{
		return std::make_unique<vk::GFXContext>(window);
	}
}
