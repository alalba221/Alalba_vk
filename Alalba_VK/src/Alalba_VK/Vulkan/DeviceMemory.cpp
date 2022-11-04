#include "pch.h"
#include "DeviceMemory.h"
#include "Device.h"
namespace vk
{
	DeviceMemory::Allocator& DeviceMemory::Allocator::SetMemSize(const uint32_t size)
	{
		m_size = size;
		return *this;
	}

	DeviceMemory::Allocator& DeviceMemory::Allocator::SetMemType(const uint32_t memType)
	{
		m_memoryType = memType;
		return *this;
	}

	DeviceMemory::Allocator& DeviceMemory::Allocator::SetMemFlags(const VkMemoryAllocateFlags flags)
	{
		m_flags = flags;
		return*this;
	}

	std::unique_ptr<DeviceMemory> DeviceMemory::Allocator::Allocate()
	{
		return std::make_unique<DeviceMemory>(m_device, m_size, m_memoryType, m_flags);
	}

	

	DeviceMemory::DeviceMemory(const Device& device, const uint32_t size, const uint32_t memoryType,
		const VkMemoryAllocateFlags flags)
		:m_device(device), m_size(size), m_memoryType(memoryType)
	{
		ALALBA_INFO("Allocate Device Memory");
		// create info
		VkMemoryAllocateFlagsInfo flagsInfo = {};
		flagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO; 
		flagsInfo.deviceMask = 0;
		flagsInfo.flags = flags; // can be set to 0; for now dont set it to 1
		flagsInfo.pNext = nullptr;


		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = m_size;
		allocInfo.memoryTypeIndex = m_memoryType;
		allocInfo.pNext = &flagsInfo;
			
		VkResult err= vkAllocateMemory(m_device.Handle(), &allocInfo, nullptr, &m_deviceMemory);
		ALALBA_ASSERT(err == VK_SUCCESS, "Allocate Device Memory Failed");
	}

	DeviceMemory::~DeviceMemory()
	{
		Clean();
	}

	void DeviceMemory::Clean()
	{
		if (m_deviceMemory != VK_NULL_HANDLE)
		{
			ALALBA_WARN("Deallocate Device Memory {0}", m_tag);
			vkFreeMemory(m_device.Handle(), m_deviceMemory, nullptr);
			m_deviceMemory = VK_NULL_HANDLE;
		}
	}

}