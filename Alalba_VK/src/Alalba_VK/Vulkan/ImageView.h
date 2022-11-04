#pragma once
#include<vulkan/vulkan.h>
namespace  vk
{
	class Image;
	class Device;

	class ImageView final
	{
	public:
		class Builder
		{
		public:
			Builder(const Device& device, const Image& image) : m_device(device), m_image(image) {};
			Builder& SetFormat(const VkFormat& format) { m_format = format; return *this; };
			Builder& SetSubresourceAspectFlags(const VkImageAspectFlags& subresourceflags) { m_subresourceflags = subresourceflags; return *this; };
			Builder& SetViewType(const VkImageViewType& viewType) { m_viewType = viewType; return *this; };
			
			Builder& SetTag(const std::string tag) { m_tag = tag; return *this; }
			std::unique_ptr<ImageView>Build()const;

		private:
			std::string m_tag;
			const Device& m_device;
			const Image& m_image;
			VkFormat m_format{};
			VkImageAspectFlags m_subresourceflags;
			VkImageViewType m_viewType{};
		};
		
	public:
		VULKAN_NON_COPIABLE(ImageView);
		ImageView(const Device& device, const VkImage& vkImage, const VkImageAspectFlags& flags,
			const VkFormat& format, const VkImageViewType viewType, const std::string& tag);

		~ImageView()
		{
			Clean();
		};
		void Clean();

		
	private:
		VULKAN_HANDLE(VkImageView, m_imageView);

		const class Device& m_device;
		VkImage m_vkImage;
		VkFormat m_format;
		VkImageViewType m_viewType;
		VkImageAspectFlags m_subresourceflags;
		//VkComponentMapping m_components;
	};

}