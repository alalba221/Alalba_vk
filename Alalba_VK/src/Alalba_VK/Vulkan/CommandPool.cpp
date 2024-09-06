#include "pch.h"
#include "CommandPool.h"
#include "Device.h"
namespace vk
{

	CommandPool::CommandPool(const Device& device, const uint32_t QFamily, const VkCommandPoolCreateFlags flags, const std::string& tag)
		:m_device(device), m_QFamily(QFamily),m_flags(flags),m_tag(tag)
	{
		LOG_INFO("Create Command Pool: {0}", m_tag);
		VkCommandPoolCreateInfo ci{};
		ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		ci.pNext = nullptr;
		ci.flags = m_flags;
		ci.queueFamilyIndex = m_QFamily;

		VkResult err = vkCreateCommandPool(m_device.Handle(), &ci, nullptr, &m_cmdPool);
		LOG_TRACE("Create Command pool: {0}", (void*)m_cmdPool);
	}
	void CommandPool::Clean()
	{
		if (m_cmdPool != VK_NULL_HANDLE)
		{
			LOG_WARN("Clean Command Pool {0}", m_tag);
			vkDestroyCommandPool(m_device.Handle(), m_cmdPool, nullptr);
			m_cmdPool = VK_NULL_HANDLE;
		}
		
	}
}