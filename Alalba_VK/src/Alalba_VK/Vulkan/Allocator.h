#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
namespace vk
{
	class Device;
	class Instance;

	//class Buffer;
	//class Image;

	class Allocator
	{
	public:
	//	class Builder
	//	{
	//	public:
	//		Builder(const Device& device, const Instance& instance) :m_device(device), m_instance(instance) {};
	//		Builder& SetTag(const std::string tag) { m_tag = tag; return *this; };
	//		Allocator* Build() const { return new Allocator(m_device, m_instance, m_tag);};
	//		//Allocator* Build() const { return new Allocator(m_device, m_instance, m_tag); };

	//	private:
	//		const class Device& m_device;
	//		const class Instance& m_instance;
	//		std::string m_tag{};
	//	};

	//public:
		VULKAN_NON_COPIABLE(Allocator);
		Allocator(const Device& device, const Instance& instance, const std::string& tag);

		VmaAllocation AllocateImage(VkImageCreateInfo imageCreateInfo, 
			VmaMemoryUsage usage, VkImage& outImage, const std::string& imagetag);
		
		VmaAllocation AllocateBuffer(VkBufferCreateInfo bufferCreateInfo,
			VmaMemoryUsage usage, VkBuffer& outBuffer, const std::string& buffertag);

		void DestroyImage(VkImage image, VmaAllocation allocation);
		void DestroyBuffer(VkBuffer buffer, VmaAllocation allocation);
			
		~Allocator() {Clean(); };
		void Clean();
		
	private:
		VULKAN_HANDLE(VmaAllocator, m_allocator);
		const class Device& m_device;
		const class Instance& m_instance;
		uint64_t s_totalAllocatedBytes = 0;

	};
}


