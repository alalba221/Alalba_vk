#include "pch.h"
#include "Fence.h"
#include "Device.h"
namespace vk
{
	Fence::Fence(const Device& device, const bool signaled, const std::string& tag)
		:m_device(device),m_tag(tag)
	{
		LOG_INFO("Create Fence: {0}", m_tag);
		VkFenceCreateInfo CI;
		CI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		CI.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
		CI.pNext = nullptr;

		VkResult err = vkCreateFence(m_device.Handle(), &CI, nullptr, &m_fence);
		ALALBA_ASSERT(err == VK_SUCCESS, "Create Fence Failed");
	}
	void Fence::Clean()
	{
		if (m_fence != VK_NULL_HANDLE)
		{
			LOG_WARN("Clean Fence {0}", m_tag);
			vkDestroyFence(m_device.Handle(), m_fence, nullptr);
			m_fence = VK_NULL_HANDLE;
		}
	}
	void Fence::Reset()
	{
		VkResult err = vkResetFences(m_device.Handle(), 1, &m_fence);
		ALALBA_ASSERT(err == VK_SUCCESS);
	}

	void Fence::Wait(uint64_t timeout) const
	{
		VkResult err = vkWaitForFences(m_device.Handle(), 1, &m_fence, VK_TRUE, timeout);
		ALALBA_ASSERT(err == VK_SUCCESS);
	}

}