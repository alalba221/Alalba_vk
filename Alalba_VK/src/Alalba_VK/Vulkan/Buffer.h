#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>


namespace vk
{
	class Device;
	class Allocator;
	class CommandPool;
	class Queue;

	class Buffer
	{
	public:
		class Builder
		{
		public:
			Builder(const Device& device, Allocator& allocator) : m_device(device), m_allocator(allocator) {};
			Builder& SetTag(const std::string& tag) { m_tag = tag; return *this; }
			Builder& SetSize(const VkDeviceSize& size) { m_size = size; return *this; }
			Builder& SetUsage(const VkBufferUsageFlags& usageFlags) { m_usageFlags = usageFlags; return *this; }
			Builder& SetVmaUsage(const VmaMemoryUsage& vmaUsage) { m_vmaUsage = vmaUsage; return *this; }

			std::unique_ptr<Buffer> Build() const 
			{
				return std::make_unique<Buffer>(m_device,m_allocator,m_size,m_usageFlags,m_vmaUsage,m_tag);
			};

		private:
			const class Device& m_device;
			class Allocator& m_allocator;
			std::string m_tag;
			VkDeviceSize m_size;
			VkBufferUsageFlags m_usageFlags;
			VmaMemoryUsage m_vmaUsage;
		};

	public:
		VULKAN_NON_COPIABLE(Buffer);
		Buffer(const Device& device, Allocator& allocator,
			const VkDeviceSize& size, const VkBufferUsageFlags& usageFlags,
			const VmaMemoryUsage& vmaUsage,
			const std::string& tag);

		// TO GPU
		void CopyDataFrom(const void* src, uint32_t sizeInByte, const Queue& q, const CommandPool& cmdPool);
		~Buffer() { Clean(); }
		void Clean();

		const VmaAllocation& GetAllocation() const { return m_allocation; }
		//VkDeviceAddress 
		uint64_t DeviceAddress() const;
		void* MapMemory();
		void UnMapMemory();
		void* Mapped() { return m_mapped; }
		
		void Flush();
	private:
		VULKAN_HANDLE(VkBuffer, m_buffer);
		const class Device& m_device;
		class Allocator& m_allocator;
		VkDeviceSize m_size;
		VkBufferUsageFlags m_usageFlags;
		VmaAllocation m_allocation;
		VmaMemoryUsage m_vmaUsage;
		VkDeviceAddress m_deviceAddr;
		void* m_mapped = nullptr;
	private:
		void MoveDataFromStagingBuffer(const Buffer& stage ,uint32_t sizeInByte, const Queue& q, const CommandPool& cmdPool);
	};
}


