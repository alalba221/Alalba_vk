#include "pch.h"
#include "Buffer.h"
#include "Device.h"
#include "Allocator.h"
#include "CommandBuffers.h"
#include "FunctionPointers.h"
namespace vk
{
	// function pointer
	static PFN_vkGetBufferDeviceAddressKHR fpGetBufferDeviceAddressKHR;
	
	Buffer::Buffer(const Device& device, Allocator& allocator, 
		const VkDeviceSize& size, const VkBufferUsageFlags& usageFlags,
		const VmaMemoryUsage& vmaUsage,
		const std::string& tag)
		:m_device(device),m_allocator(allocator),m_size(size), m_usageFlags(usageFlags),
		m_vmaUsage(vmaUsage),
		m_tag(tag)
	{
		LOG_INFO("Create Buffer: {0}", m_tag);
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

		GET_DEVICE_PROC_ADDR(m_device.Handle(), GetBufferDeviceAddressKHR);

	}
	// copy from memory  to gpu_only or (cpu_to_gpu) using a cpu_only 
	void Buffer::CopyDataFrom(const void* src, size_t sizeInByte, const Queue& q, const CommandPool& cmdPool)
	{
		// 1. create staging buffer 
		std::unique_ptr<Buffer>stagingBuffer = Buffer::Builder(m_device, m_allocator)
			.SetTag("Staging Buffer")
			.SetSize(sizeInByte)
			.SetUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
			.SetVmaUsage(VMA_MEMORY_USAGE_CPU_ONLY)
			.Build();

		// 2. copy data to staging buffer
		void* data = stagingBuffer->MapMemory();
		memcpy(data, src, (size_t)sizeInByte);
		stagingBuffer->UnMapMemory();

		// 3. copy staging buffer to vertex buffer: using command pool for graphics
		MoveDataFromStagingBuffer(*stagingBuffer, sizeInByte, q, cmdPool);
		// 4. Clear Staging buffer
		stagingBuffer->Clean();
	}

	void Buffer::Clean()
	{
		if (m_buffer != VK_NULL_HANDLE)
		{
			LOG_WARN("Clean Buffer {0}", m_tag);

			if (m_mapped != nullptr)
				UnMapMemory();

			m_allocator.DestroyBuffer(m_buffer, m_allocation);
			m_buffer = VK_NULL_HANDLE;
		}
	}
	
	uint64_t Buffer::DeviceAddress() const
	{
		VkBufferDeviceAddressInfoKHR bufferDeviceAI{};
		bufferDeviceAI.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		bufferDeviceAI.buffer = m_buffer;
		return fpGetBufferDeviceAddressKHR(m_device.Handle(), &bufferDeviceAI);
	}

	void* Buffer::MapMemory()
	{
		//void* mappedMemory;
		vmaMapMemory(m_allocator.Handle(), m_allocation, &m_mapped);
		return m_mapped;
	}
	void Buffer::UnMapMemory()
	{
		vmaUnmapMemory(m_allocator.Handle(), m_allocation);
		m_mapped = nullptr;
	}
	void Buffer::Flush()
	{
		VkResult err = vmaFlushAllocation(m_allocator.Handle(), m_allocation, 0, VK_WHOLE_SIZE);
		ALALBA_ASSERT(err == VK_SUCCESS, "Flush buffer failed");
	}
	void Buffer::MoveDataFromStagingBuffer(const Buffer& stage, uint32_t sizeInByte, const Queue& q, const CommandPool& cmdPool)
	{
		std::unique_ptr<CommandBuffers>copyCmdBuffer = CommandBuffers::Allocator(m_device, cmdPool)
			.SetTag("Command Buffers for Copy Buffer: "+m_tag)
			.OneTimeSubmit(true)
			.SetSize(1)
			.Allocate();
		// recording command buffer
		VkBufferCopy copyRegion = {};
		copyRegion.size = sizeInByte;
		copyCmdBuffer->BeginRecording(0);
		VkCommandBuffer cmdbuffer = (*copyCmdBuffer)[0];
		vkCmdCopyBuffer(cmdbuffer, stage.Handle(), m_buffer, 1, &copyRegion);
		copyCmdBuffer->EndRecording(0);
		copyCmdBuffer->Flush(0, q);
	}
}