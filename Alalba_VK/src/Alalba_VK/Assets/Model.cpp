#include "pch.h"
#include "Model.h"
#include "Mesh.h"
#include "Alalba_VK/Vulkan/Buffer.h"
#include "Alalba_VK/Core/Application.h"
#include "Alalba_VK/Vulkan/Allocator.h"
#include "Alalba_VK/Vulkan/CommandPool.h"

namespace Alalba
{
	vk::Allocator* Model::s_allocator = nullptr;
	vk::CommandPool* Model::s_commandPool = nullptr;

	Model* Model::Create(const std::string file)
	{
		// Static members
		if (s_allocator == nullptr)
			s_allocator = new vk::Allocator(Application::Get().GetDevice(), Alalba::Application::Get().GetVulkanInstance(), "Mesh Allocator");
		if (s_commandPool == nullptr)
			s_commandPool = new vk::CommandPool(Application::Get().GetDevice(),
				Alalba::Application::Get().GetVulkanInstance().GetPhysicalDevice().GetQFamilies().graphics.value(),
				VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
				"Model CommandPool");

		return new Mesh(file);
	}
}

