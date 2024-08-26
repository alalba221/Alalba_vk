#include "pch.h"
#include "Semaphore.h"
#include "Device.h"
namespace vk
{
	Semaphore::Semaphore(const Device& device, const std::string& tag)
		:m_device(device),m_tag(tag)
	{
		LOG_INFO("Create Semaphore: {0}", m_tag);
		VkSemaphoreCreateInfo CI{};
		CI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		CI.pNext = nullptr;
		// CI.flags  is reserved for future use.
		VkResult err = vkCreateSemaphore(m_device.Handle(), &CI, nullptr, &m_semaphore);

		ALALBA_ASSERT(err == VK_SUCCESS,"Create Semaphore Failed");
	}

	void Semaphore::Clean()
	{
		if (m_semaphore != VK_NULL_HANDLE)
		{
			LOG_WARN("Clean Semaphore {0}", m_tag);
			vkDestroySemaphore(m_device.Handle(), m_semaphore, nullptr);
			m_semaphore = VK_NULL_HANDLE;
		}
	}
}