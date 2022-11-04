#include "pch.h"
#include "Buffer.h"
#include "Device.h"
#include "Allocator.h"
namespace vk
{
	Buffer::Buffer(const Device& device, Allocator& allocator, 
		const VkDeviceSize& size, const VkBufferUsageFlags& usageFlags,
		const VmaMemoryUsage& vmaUsage,
		const std::string& tag)
		:m_device(device),m_allocator(allocator),m_size(size), m_usageFlags(usageFlags),
		m_vmaUsage(vmaUsage),
		m_tag(tag)
	{
		ALALBA_INFO("Create Buffer: {0}", m_tag);
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.pNext = nullptr;
		bufferInfo.size = m_size;
		bufferInfo.usage = m_usageFlags;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		// VkBufferCreateFlags    flags;
		// uint32_t               queueFamilyIndexCount;
		// const uint32_t* pQueueFamilyIndices;
		m_allocation = m_allocator.AllocateBuffer(bufferInfo, m_vmaUsage, m_buffer, m_tag);
	}
	void Buffer::Clean()
	{
		if (m_buffer != VK_NULL_HANDLE)
		{
			ALALBA_WARN("Clean Buffer {0}", m_tag);
			m_allocator.DestroyBuffer(m_buffer, m_allocation);
			m_buffer = VK_NULL_HANDLE;
		}
	}
}