#include "pch.h"
#include "ImageView.h"
#include "Device.h"
#include "Image.h"
namespace vk
{
	std::unique_ptr<ImageView> ImageView::Builder::Build()const
	{
		return std::make_unique<ImageView>(m_device, m_image.Handle(), m_subresourceflags, m_format, m_viewType, m_tag);
	}

	ImageView::ImageView(const Device& device, const VkImage& vkImage, const VkImageAspectFlags& subresourceflags,
		const VkFormat& format, const VkImageViewType viewType, const std::string& tag)
		:m_device(device), m_vkImage(vkImage), m_subresourceflags(subresourceflags), m_format(format), m_viewType(viewType), m_tag(tag)
	{
		// create info	
		ALALBA_INFO("	Create Image View: {0}",m_tag);
		VkImageSubresourceRange subresourceRange{};
		subresourceRange.aspectMask = m_subresourceflags;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = 1;

		VkImageViewCreateInfo createinfo{};
		createinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createinfo.pNext = nullptr;
	/*	createinfo.components.r = VK_COMPONENT_SWIZZLE_R;
		createinfo.components.g = VK_COMPONENT_SWIZZLE_G;
		createinfo.components.b = VK_COMPONENT_SWIZZLE_B;
		createinfo.components.a = VK_COMPONENT_SWIZZLE_A;*/
		createinfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createinfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createinfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createinfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createinfo.image = vkImage;
		createinfo.viewType = m_viewType;
		createinfo.format = m_format;
		createinfo.subresourceRange = subresourceRange;
		//VkImageViewCreateFlags     flags;
		
		VkResult err;
		err = vkCreateImageView(m_device.Handle(), &createinfo, nullptr, &m_imageView);
		ALALBA_ASSERT(err == VK_SUCCESS);
	}
	void ImageView::Clean()
	{
		if (m_imageView != VK_NULL_HANDLE)
		{
			ALALBA_WARN("Clean Image View {0}", m_tag);
			vkDestroyImageView(m_device.Handle(), m_imageView, nullptr);
			m_imageView = VK_NULL_HANDLE;
		}
	}
}
