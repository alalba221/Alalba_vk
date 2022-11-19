#include "pch.h"
#include "Allocator.h"
#include "Device.h"
#include "Instance.h"
#include "Buffer.h"
#include "CommandBuffers.h"
#include "Alalba_VK/Utilities/StringUtils.h"
namespace vk
{
	Allocator::Allocator(const Device& device, const Instance& instance)
		:m_device(device),m_instance(instance)
	{
		ALALBA_INFO("Create VMA Allocator");
		VmaAllocatorCreateInfo CI = {};
		//CI.flags
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
		ALALBA_INFO ("VulkanAllocator: allocating image {1}; size = {0}", Utils::BytesToString(allocInfo.size), imagetag);
		{
			s_totalAllocatedBytes += allocInfo.size;
			ALALBA_INFO("VulkanAllocator: total allocated since start is {0}",  Utils::BytesToString(s_totalAllocatedBytes));
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

		ALALBA_INFO("VulkanAllocator : allocating buffer{1}; size = {0}",Utils::BytesToString(allocInfo.size),buffertag);
		{
			s_totalAllocatedBytes += allocInfo.size;
			ALALBA_INFO("VulkanAllocator: total allocated since start is {0}", Utils::BytesToString(s_totalAllocatedBytes));
		}

		return allocation;

	}
	void Allocator::DestroyImage(VkImage image, VmaAllocation allocation)
	{
		ALALBA_ASSERT(image);
		ALALBA_ASSERT(allocation);
		vmaDestroyImage(m_allocator, image, allocation);
	}

	void Allocator::DestroyBuffer(VkBuffer buffer, VmaAllocation allocation)
	{
		ALALBA_ASSERT(buffer);
		ALALBA_ASSERT(allocation);
		vmaDestroyBuffer(m_allocator, buffer, allocation);
	}

	void* Allocator::MapMemory(VmaAllocation allocation)
	{
		void* mappedMemory;
		vmaMapMemory(m_allocator, allocation, &mappedMemory);
		return mappedMemory;
	}

	void Allocator::UnMapMemory(VmaAllocation allocation)
	{
		vmaUnmapMemory(m_allocator, allocation);
	}

	void Allocator::CopyDataToGPU(void* src, Buffer& dst, uint32_t sizeInByte, const Queue& q, const CommandPool& cmdPool)
	{
		// 1. create staging buffer 
		std::unique_ptr<Buffer>m_stagingVectexBuffer = Buffer::Builder(m_device, *this)
			.SetTag("Staging Buffer")
			.SetSize(sizeInByte)
			.SetUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
			.SetVmaUsage(VMA_MEMORY_USAGE_CPU_TO_GPU)
			.Build();
		
		// 2. copy data to staging buffer
		void* data = MapMemory(m_stagingVectexBuffer->GetAllocation());
		memcpy(data, src, (size_t)sizeInByte);
		UnMapMemory(m_stagingVectexBuffer->GetAllocation());

		// 3. copy staging buffer to vertex buffer: using command pool for graphics
		std::unique_ptr<CommandBuffers>m_copyCmdBuffer = CommandBuffers::Allocator(m_device, cmdPool)
			.SetTag("Command Buffers for Copy")
			.SetSize(1)
			.Allocate();

		{
			// recording command buffer
			VkBufferCopy copyRegion = {};
			copyRegion.size = sizeInByte;
			m_copyCmdBuffer->BeginRecording(0);
			VkCommandBuffer cmdbuffer = (*m_copyCmdBuffer.get())[0];
			vkCmdCopyBuffer(cmdbuffer, m_stagingVectexBuffer->Handle(), dst.Handle(), 1, &copyRegion);
			m_copyCmdBuffer->EndRecording(0);
			m_copyCmdBuffer->Flush(0, q);
		}
		m_stagingVectexBuffer->Clean();
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