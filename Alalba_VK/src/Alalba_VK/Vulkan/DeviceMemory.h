/*********
Device memory is memory that is visible to the device — for example the contents of the image or
buffer objects, which can be natively used by the device.
************/

#pragma once
#include <vulkan/vulkan.h>
#include "VkCommon.h"

namespace vk
{
	 // class PhysicalDevice;
	class Device;

	class DeviceMemory
	{
	public:
		class Allocator
		{
		public:
			Allocator(const Device& device) :m_device(device) {};
			Allocator& SetMemSize(const uint32_t size);
			Allocator& SetMemType(const uint32_t memType);// Need  to cooperate GetBufferMemoryRequirements
			// Need  to cooperate with Buffer USAGE FLAG,
			// can be set to 0; for now dont set it to 1
			Allocator& SetMemFlags(const VkMemoryAllocateFlags flags);
			std::unique_ptr<DeviceMemory> Allocate();

		private:
			const class Device& m_device;
			uint32_t m_size = 0;
			uint32_t m_memoryType = -1;
			VkMemoryAllocateFlags m_flags = 0;
		};

	public:
		VULKAN_NON_COPIABLE(DeviceMemory);
		DeviceMemory(const Device& device, const uint32_t size, const uint32_t memoryType,
			const VkMemoryAllocateFlags flags);
		~DeviceMemory();
		void Clean();

	private:
		VULKAN_HANDLE(VkDeviceMemory,m_deviceMemory)
		//const class PhysicalDevice& m_device;
		const class Device& m_device;
		uint32_t m_size;
		uint32_t m_memoryType;
	};

}
