#include "pch.h"
#include "Sampler.h"
#include "Device.h"

namespace vk {

  Sampler::Sampler(const Device& device, VkFilter filter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode addressMode, const std::string& tag)
		:m_device(device),m_tag(tag)
	{
		LOG_INFO("Create Sampler: {0}", m_tag);

    VkSamplerCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    //const void* pNext;
    //VkSamplerCreateFlags    flags;
    info.magFilter = filter;
    info.minFilter = filter;
    info.mipmapMode = mipmapMode;
    info.addressModeU = addressMode;
    info.addressModeV = addressMode;
    info.addressModeW = addressMode;
    
    info.anisotropyEnable = VK_TRUE;
    info.maxAnisotropy = 1.0f;
    info.compareEnable = VK_FALSE;
    info.compareOp = VK_COMPARE_OP_ALWAYS;
    info.mipLodBias = 0.0f;
    info.minLod = 0.0f;
    info.maxLod = 0.0f;
    info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    info.unnormalizedCoordinates = VK_FALSE;

    VkResult err = vkCreateSampler(m_device.Handle(), &info, nullptr, &m_sampler);
    ALALBA_ASSERT(err == VK_SUCCESS);
	}

  void Sampler::Clean()
  {
    if (m_sampler != VK_NULL_HANDLE)
    {
      LOG_WARN("Clean Sampler: {0}", m_tag);
      vkDestroySampler(m_device.Handle(), m_sampler, nullptr);
      m_sampler = VK_NULL_HANDLE;
    }
  }
}