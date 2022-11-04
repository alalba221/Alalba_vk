#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
namespace vk
{
	class Device;
	class Instance;
	// used by CopyDataToGPU
	class Queue;
	class Buffer;
	class CommandPool;

	class Allocator
	{
	public:
		class Builder
		{
		public:
			Builder(const Device& device, const Instance& instance) :m_device(device), m_instance(instance) {};
			std::unique_ptr<Allocator>Build() const { return std::make_unique<Allocator>(m_device, m_instance);};

		private:
			const class Device& m_device;
			const class Instance& m_instance;
		};

	public:
		VULKAN_NON_COPIABLE(Allocator);
		Allocator(const Device& device, const Instance& instance);

		VmaAllocation AllocateImage(VkImageCreateInfo imageCreateInfo, 
			VmaMemoryUsage usage, VkImage& outImage, const std::string& imagetag);
		
		VmaAllocation AllocateBuffer(VkBufferCreateInfo bufferCreateInfo,
			VmaMemoryUsage usage, VkBuffer& outBuffer, const std::string& buffertag);

		void DestroyImage(VkImage image, VmaAllocation allocation);
		void DestroyBuffer(VkBuffer buffer, VmaAllocation allocation);

		void* MapMemory(VmaAllocation allocation);
		void UnMapMemory(VmaAllocation allocation);

		void CopyDataToGPU(void* src, Buffer& dst, uint32_t sizeInByte, const Queue& q, const CommandPool& cmdPool);

		~Allocator() {Clean(); };
		void Clean();
		
	private:
		VULKAN_HANDLE(VmaAllocator, m_allocator);
		const class Device& m_device;
		const class Instance& m_instance;

		uint64_t s_totalAllocatedBytes = 0;

	};
}


