#include "pch.h"
#include "DescriptorPool.h"
#include "Device.h"
namespace vk
{
	DescriptorPool::DescriptorPool(const Device& device, 
		VkDescriptorPoolCreateFlags flags, uint32_t maxSets, const std::vector<VkDescriptorPoolSize>& poolSizes,
		const std::string& tag)
		:m_device(device), m_poolFlags(flags), m_tag(tag)
	{
		LOG_INFO("Create Descriptor Pool:{0}", m_tag);
		VkDescriptorPoolCreateInfo ci{};
		ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		ci.pNext = nullptr;
		ci.flags = m_poolFlags;
		ci.maxSets = maxSets;
		ci.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		ci.pPoolSizes = poolSizes.data();

		VkResult err;
		err = vkCreateDescriptorPool(m_device.Handle(), &ci, nullptr, &m_descriptorPool);
		ALALBA_ASSERT(err == VK_SUCCESS, "failed to create descriptor pool!");
	}

	void DescriptorPool::Clean()
	{
		if (m_descriptorPool != VK_NULL_HANDLE)
		{
			LOG_WARN("Clean Descriptor Pool: {0}", m_tag);
			vkDestroyDescriptorPool(m_device.Handle(), m_descriptorPool, nullptr);
			m_descriptorPool = VK_NULL_HANDLE;
		}
	}
}