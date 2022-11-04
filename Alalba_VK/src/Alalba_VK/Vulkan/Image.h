#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
namespace vk
{
	class Device;
	class Allocator;

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
			std::string m_tag;
		};

	public:
		VULKAN_NON_COPIABLE(Image);

		Image(const Device& device, Allocator& allocator,
			const VkImageType imageType, 
			const VkImageUsageFlags usageFlags, const VkExtent3D entent, const VkFormat format, const VkImageTiling tilling,
			const std::string& tag);
		const VkImageType& GetType() const { return m_imageType; };
		const VkExtent3D& GetExtent() const { return m_extent; }
		const VkFormat& GetFormat() const { return m_format; };

		~Image() { Clean(); };
		void Clean();

	private:
		VULKAN_HANDLE(VkImage, m_image);
		const class Device& m_device;
		class Allocator& m_allocator;
		VkImageType m_imageType;
		VkExtent3D m_extent;
		VkFormat m_format;
		VkImageUsageFlags m_usageFlags;
		VmaAllocation m_allocation;
	};
}
