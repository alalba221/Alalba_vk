#include "pch.h"
#include "Queue.h"
namespace vk
{
	std::vector<VkDeviceQueueCreateInfo> Queue::s_qCreateInfos{};

	Queue::Configurer& Queue::Configurer::SetQFamily(const uint32_t queueFamily)
	{
		m_qFamily = queueFamily;
		return *this;
	}

	Queue::Configurer& Queue::Configurer::SetPriority(const float priority)
	{
		m_priority = priority;
		return *this;
	}

	std::unique_ptr<Queue> Queue::Configurer::Configure()
	{
		return std::make_unique<Queue>(m_qFamily, m_priority);
	}

	Queue::Queue(const uint32_t qFamily, const float priority)
		:m_queueFamily(qFamily),m_priority(priority)
	{
		m_createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		m_createInfo.queueFamilyIndex = m_queueFamily;
		m_createInfo.queueCount = 1;
		m_createInfo.pQueuePriorities = &m_priority;

		s_qCreateInfos.push_back(m_createInfo);
	}
	
}