#include "pch.h"
#include "Image.h"
#include "Device.h"
#include "Allocator.h"
#include "Buffer.h"
#include "CommandBuffers.h"
namespace vk
{
  std::unique_ptr<Image>Image::Builder::Build()const
  {
    return std::make_unique<Image>(m_device, m_allocator, m_imageType, m_usageFlags, m_extent, m_format, m_tilling,
      m_sharingMode,m_tag);
  }

  void Image::CopyImageFrom(void* src, uint32_t sizeInByte, const Queue& q, const CommandPool& cmdPool)
  {
    // 1. create staging buffer 
    std::unique_ptr<Buffer>stagingBuffer = Buffer::Builder(m_device, m_allocator)
      .SetTag("Staging Buffer")
      .SetSize(sizeInByte)
      .SetUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
      .SetVmaUsage(VMA_MEMORY_USAGE_CPU_ONLY)
      .Build();

    // 2. Copy data to staging buffer
    void* data = stagingBuffer->MapMemory();
    memcpy(data, src, (size_t)sizeInByte);
    stagingBuffer->UnMapMemory();

    // 3. Layout transitions
    TransitionImageLayout(cmdPool, q, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    // 4. Copy staging buffer to image buffer: using command pool for graphics
    MoveDataFromStagingBuffer(*stagingBuffer, sizeInByte, q, cmdPool);
    
    // 5. Layout transitions
    TransitionImageLayout(cmdPool, q, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // 6. clear staging buffer
    stagingBuffer->Clean();
  }

	Image::Image(const Device& device, Allocator& allocator,
    const VkImageType imageType, 
    const VkImageUsageFlags usageFlags, const VkExtent3D entent, const VkFormat format, const VkImageTiling tilling,
    const VkSharingMode& sharingMode,
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
    createinfo.sharingMode = sharingMode;
    
    /// TODO: 
    // If compute and graphics queue family indices differ, we create an image that can be shared between them
    // This can result in worse performance than exclusive sharing mode, but save some synchronization to keep the sample simple
    std::vector<uint32_t> queueFamilyIndices;
    if (sharingMode == VK_SHARING_MODE_CONCURRENT)
    {
      queueFamilyIndices = {
        m_device.GetGraphicsQ().GetFamily(),
        m_device.GetComputeQ().GetFamily()
      };
      createinfo.queueFamilyIndexCount = queueFamilyIndices.size();
      createinfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }
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
  void Image::TransitionImageLayout(const CommandPool& cmdPool, const Queue& q, VkImageLayout newLayout)
  {
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = m_currentlLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_image;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
      barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

      if (HasStencilComponent())
      {
        barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
      }
    }
    else
    {
      barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (m_currentlLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

      sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (m_currentlLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (m_currentlLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

      sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    // for compute shader target texture
    else if (m_currentlLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_GENERAL)
    {
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = 0;
      
      barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.layerCount = 1;

      sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
      destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }
    else
    {
      ALALBA_ASSERT("Unsupported layout");
    }

    std::unique_ptr<CommandBuffers>m_copyCmdBuffer = CommandBuffers::Allocator(m_device, cmdPool)
      .SetTag("Command Buffers for Layout Transition")
      .OneTimeSubmit(true)
      .SetSize(1)
      .Allocate();
    {
      // recording command buffer
      m_copyCmdBuffer->BeginRecording(0);
      VkCommandBuffer cmdbuffer = (*m_copyCmdBuffer)[0];
      vkCmdPipelineBarrier(cmdbuffer, 
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);
      m_copyCmdBuffer->EndRecording(0);
      m_copyCmdBuffer->Flush(0, q);
    }
    m_currentlLayout = newLayout;

  }
  void Image::MoveDataFromStagingBuffer(const Buffer& stage, uint32_t sizeInByte, const Queue& q, const CommandPool& cmdPool)
  {
    std::unique_ptr<CommandBuffers>copyCmdBuffer = CommandBuffers::Allocator(m_device, cmdPool)
      .SetTag("Command Buffers for Copy Image: "+m_tag)
      .OneTimeSubmit(true)
      .SetSize(1)
      .Allocate();
    // recording command buffer
    VkBufferImageCopy  region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        m_extent.width,
        m_extent.height,
        1
    };
    copyCmdBuffer->BeginRecording(0);
    VkCommandBuffer cmdbuffer = (*copyCmdBuffer)[0];
    vkCmdCopyBufferToImage(cmdbuffer, stage.Handle(), m_image,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    copyCmdBuffer->EndRecording(0);
    copyCmdBuffer->Flush(0, q);
  }
}