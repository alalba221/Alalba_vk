#include "pch.h"
#include "Image.h"
#include "Device.h"
#include "Allocator.h"
namespace vk
{
  std::unique_ptr<Image>Image::Builder::Build()const
  {
    return std::make_unique<Image>(m_device, m_allocator, m_imageType, m_usageFlags, m_extent, m_format, m_tilling,m_tag);
  }

	Image::Image(const Device& device, Allocator& allocator,
    const VkImageType imageType, 
    const VkImageUsageFlags usageFlags, const VkExtent3D entent, const VkFormat format, const VkImageTiling tilling,
    const std::string& tag)
		:m_device(device),m_allocator(allocator), m_imageType(imageType), m_usageFlags(usageFlags), m_extent(entent), m_format(format), m_tag(tag)
	{
    ALALBA_INFO("Create Image: {0}, size:({1},{2})", m_tag,m_extent.width,m_extent.height);
    // create info
		VkImageCreateInfo createinfo{};
    createinfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    
    createinfo.imageType = m_imageType;
    createinfo.extent = m_extent;
    createinfo.format = m_format;

    createinfo.tiling = tilling;
    createinfo.usage = m_usageFlags;
    createinfo.mipLevels = 1;
    createinfo.arrayLayers = 1;
    createinfo.samples = VK_SAMPLE_COUNT_1_BIT; 
    createinfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createinfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // createinfo.queueFamilyIndexCount = 
    // createinfo.pQueueFamilyIndices = 
    // createinfo.flags =
    // createinfo.pNext =
    
    /// GPU_ONLY
    m_allocation= m_allocator.AllocateImage(createinfo, VMA_MEMORY_USAGE_GPU_ONLY, m_image, m_tag);

		
	}

  void Image::Clean()
  {
    if (m_image != VK_NULL_HANDLE)
    {
      ALALBA_WARN("Clean Image {0}", m_tag);
      m_allocator.DestroyImage(m_image, m_allocation);
      m_image = VK_NULL_HANDLE;
    }
  }
}