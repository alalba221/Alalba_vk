#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include "Alalba_VK/Vulkan/CommandPool.h"
namespace vk
{
	class Device;
	class Allocator;
	class Queue;
	class CommandPool;
	class Buffer;

	class Image
	{
	public:
 
		class Builder 
		{
		public:
			Builder(const Device& device, Allocator& allocator) : m_device(device),m_allocator(allocator) {};
			Builder& SetTag(const std::string tag) { m_tag = tag; return *this; }
			Builder& SetImgType(const VkImageType imageType) { m_imageType = imageType; return *this; };
			Builder& SetImgExtent(const VkExtent3D entent) { m_extent = entent; return *this; };
			Builder& SetImageFormat(const VkFormat format) { m_format = format; return*this; }
			Builder& SetUsageFlags(const VkImageUsageFlags usageFlags) { m_usageFlags = usageFlags; return *this; }
			Builder& SetImageTiling(const VkImageTiling tilling) { m_tilling = tilling; return *this; }
			Builder& SetSharingMode(const VkSharingMode sharingMode) { m_sharingMode = sharingMode; return *this; }
			std::unique_ptr<Image>Build()const;

		private:
			const class Device& m_device;
			// allocator is not a const, as s_total size varies after each allocation
			class Allocator& m_allocator;

			VkImageType m_imageType;
			VkExtent3D m_extent = {0,0,0};
			VkFormat m_format;
			VkImageUsageFlags m_usageFlags;
			VkImageTiling m_tilling;
			VkSharingMode m_sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			std::string m_tag;
		};

	public:
		VULKAN_NON_COPIABLE(Image);

		Image(const Device& device, Allocator& allocator,
			const VkImageType imageType, 
			const VkImageUsageFlags usageFlags, const VkExtent3D entent, const VkFormat format, const VkImageTiling tilling,
			const VkSharingMode& sharingMode,
			const std::string& tag);
		const VkImageType& GetType() const { return m_imageType; };
		const VkExtent3D& GetExtent() const { return m_extent; }
		const VkFormat& GetFormat() const { return m_format; };
		const VkImageTiling& GetTiling() const { return m_tilling; };
		
		/** 
		* @ breif only for transfering texture TO GPU, since the finall imagelayout is finallly set as VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		*/
		void CopyImageFromData(void* src, uint32_t sizeInByte, const Queue& q, const CommandPool& cmdPool);
		
		~Image() { Clean(); };
		void Clean();
		// used by depth image so move it from private to public
		void TransitionImageLayout(const CommandPool& cmdPool, const Queue& q, VkImageLayout newLayout);
		VkImageLayout Layout() const { return m_currentlLayout; }

	private:
		VULKAN_HANDLE(VkImage, m_image);
		const class Device& m_device;
		class Allocator& m_allocator;
		VkImageType m_imageType;
		VkExtent3D m_extent;
		VkFormat m_format;
		VkImageTiling m_tilling;
		VkImageUsageFlags m_usageFlags;
		VmaAllocation m_allocation;
		VkImageLayout m_currentlLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	private:
		bool HasStencilComponent() 
		{
			return m_format == VK_FORMAT_D32_SFLOAT_S8_UINT || m_format == VK_FORMAT_D24_UNORM_S8_UINT;
		}
		void MoveDataFromStagingBuffer(const Buffer& stage, uint32_t sizeInByte, const Queue& q, const CommandPool& cmdPool);
	};
}
