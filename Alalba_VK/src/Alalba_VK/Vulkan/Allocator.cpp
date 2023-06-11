#include "pch.h"
#include "Allocator.h"
#include "Device.h"
#include "Instance.h"
#include "Buffer.h"
#include "CommandBuffers.h"
#include "Alalba_VK/Utilities/StringUtils.h"
namespace vk
{
	Allocator::Allocator(const Device& device, const Instance& instance, const std::string& tag)
		:m_device(device),m_instance(instance),m_tag(tag)
	{
		ALALBA_INFO("Create VMA Allocator: {0}", m_tag);
		VmaAllocatorCreateInfo CI = {};
		CI.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		CI.physicalDevice = m_instance.GetPhysicalDevice().Handle();
		CI.device = m_device.Handle();
		CI.instance = m_instance.Handle();
		//CI.preferredLargeHeapBlockSize
		//CI.pAllocationCallbacks
		//CI.pDeviceMemoryCallbacks
		//CI.pHeapSizeLimit
		//CI.pVulkanFunctions
		//CI.vulkanApiVersion
		//CI.pTypeExternalMemoryHandleTypes
		vmaCreateAllocator(&CI, &m_allocator);
	}

	VmaAllocation Allocator::AllocateImage(VkImageCreateInfo imageCreateInfo, VmaMemoryUsage usage, VkImage& outImage, const std::string& imagetag)
	{
		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.usage = usage;

		VmaAllocation allocation;
		VkResult err=vmaCreateImage(m_allocator, &imageCreateInfo, &allocCreateInfo, &outImage, &allocation, nullptr);
		ALALBA_ASSERT(err == VK_SUCCESS, "Allocate Image Failed");
		// TODO: Tracking
		VmaAllocationInfo allocInfo;
		vmaGetAllocationInfo(m_allocator, allocation, &allocInfo);
		ALALBA_INFO("VulkanAllocator: {0} allocating image: {1}; size = {2}", m_tag, imagetag, Utils::BytesToString(allocInfo.size));
		{
			s_totalAllocatedBytes += allocInfo.size;
			ALALBA_INFO("VulkanAllocator: {0} total allocated since start is {1}", m_tag, Utils::BytesToString(s_totalAllocatedBytes));
		}
		return allocation;
	}

	VmaAllocation Allocator::AllocateBuffer(VkBufferCreateInfo bufferCreateInfo, VmaMemoryUsage usage, VkBuffer& outBuffer, const std::string& buffertag)
	{
		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.usage = usage;

		VmaAllocation allocation;
		VkResult err = vmaCreateBuffer(m_allocator, &bufferCreateInfo, &allocCreateInfo, &outBuffer, &allocation, nullptr);
		ALALBA_ASSERT(err == VK_SUCCESS, "Allocate Buffer Failed");
		// TODO: Tracking
		VmaAllocationInfo allocInfo{};
		vmaGetAllocationInfo(m_allocator, allocation, &allocInfo);

		ALALBA_INFO("VulkanAllocator: {0} allocating buffer: {1}; size = {2}",m_tag, buffertag, Utils::BytesToString(allocInfo.size));
		{
			s_totalAllocatedBytes += allocInfo.size;
			ALALBA_INFO("VulkanAllocator: {0} total allocated since start is {1}",m_tag, Utils::BytesToString(s_totalAllocatedBytes));
		}

		return allocation;
	}
	void Allocator::DestroyImage(VkImage image, VmaAllocation allocation)
	{
		ALALBA_ASSERT(image, "No image");
		ALALBA_ASSERT(allocation,"No allocator");
		vmaDestroyImage(m_allocator, image, allocation);
	}

	void Allocator::DestroyBuffer(VkBuffer buffer, VmaAllocation allocation)
	{
		ALALBA_ASSERT(buffer);
		ALALBA_ASSERT(allocation);
		vmaDestroyBuffer(m_allocator, buffer, allocation);
	}

	void Allocator::Clean()
	{
		if (m_allocator != nullptr)
		{
			ALALBA_WARN("Clean Allocotor {0}", m_tag);
			vmaDestroyAllocator(m_allocator);
			m_allocator = nullptr;
		}
	}
}