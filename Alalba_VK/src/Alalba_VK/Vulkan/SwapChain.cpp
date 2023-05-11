#include "pch.h"
#include "SwapChain.h"

#include "Device.h"
#include "Surface.h"

// For swapchain size, later must get this file out off
#include "Alalba_VK/Core/Window.h"

namespace vk
{
  
  VkSurfaceFormatKHR SwapChain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const
  {
    for (const auto& availableFormat : availableFormats)
    {
      if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
      {
        return availableFormat;
      }
    }
  }
  VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const
  {
    for (const auto& availablePresentMode : availablePresentModes)
    {
      if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        return availablePresentMode;
      }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
  {
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
      return capabilities.currentExtent;
    }
    else
    {
      int width, height;
    
      glfwGetFramebufferSize((GLFWwindow*)(m_surface.GetWindow().GetNativeWindow()), &width, &height);

      VkExtent2D actualExtent = {
          static_cast<uint32_t>(width),
          static_cast<uint32_t>(height)
      };

      actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
      actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

      return actualExtent;
    }
  }

  const ImageView& SwapChain::GetImageView(const uint32_t index) const
  {
    if (index < m_imageViews.size())
      return *(m_imageViews[index]);
    else
      ALALBA_ASSERT("Not valid index");
  }

  SwapChain::Builder& SwapChain::Builder::SetPresentMode(const VkPresentModeKHR& presentMode)
  {
    m_presentMode = presentMode;
    return *this;
  }

  SwapChain::Builder& SwapChain::Builder::SetImgSharingMode(const VkSharingMode& shareMode)
  {
    m_imageShareMode = shareMode;
    return *this;
  }

  std::unique_ptr<SwapChain>   SwapChain::Builder::Build()
  {
    Surface::SupportDetails surfaceDetails = m_surface.FindDetails();
    
    // check the present mode value  if reasonable
    std::vector<VkPresentModeKHR>::iterator prit;
    prit = std::find(surfaceDetails.presentModes.begin(), surfaceDetails.presentModes.end(), m_presentMode);
    if (prit == surfaceDetails.presentModes.end())
      m_presentMode = VK_PRESENT_MODE_FIFO_KHR;

    return std::make_unique<SwapChain>(m_device, m_surface, m_presentMode, m_imageShareMode);
  }

	SwapChain::SwapChain(const Device& device, const Surface& surface,
    const VkPresentModeKHR presentMode,
    const VkSharingMode imageShareMode)
		:m_device(device),m_surface(surface)
	{
    ALALBA_INFO("Create Swap Chain");

    Surface::SupportDetails surfaceDetails = m_surface.FindDetails();
    
    // minImage count
    uint32_t minImageCount = surfaceDetails.capabilities.minImageCount + 1;
    if (surfaceDetails.capabilities.maxImageCount > 0 && minImageCount > surfaceDetails.capabilities.maxImageCount) {
      minImageCount = surfaceDetails.capabilities.maxImageCount;
    }
    // Color space and image format
    // const auto surfaceFormat = ChoosefaceFormat(surfaceDetails.formats);
    // const auto actualPresentMode = ChooseSwapPresentMode(surfaceDetails.presentModes);
    // const auto extent = ChooseSwapExtent(surfaceDetails.capabilities);
    VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(surfaceDetails.formats);
    VkPresentModeKHR actualPresentMode = ChooseSwapPresentMode(surfaceDetails.presentModes);
    m_extent = ChooseSwapExtent(surfaceDetails.capabilities);

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface.Handle();
    createInfo.minImageCount = minImageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = m_extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    // assume graphics queue support present
    createInfo.imageSharingMode = imageShareMode;
    createInfo.queueFamilyIndexCount = (imageShareMode==VK_SHARING_MODE_EXCLUSIVE)?0:2; // 0 for VK_SHARING_MODE_EXCLUSIVE, 2 for VK_SHARING_MODE_CONCURRENT 
    createInfo.pQueueFamilyIndices = nullptr; // Optional
    createInfo.preTransform = surfaceDetails.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult err;
    err = vkCreateSwapchainKHR(m_device.Handle(), &createInfo, nullptr, &m_swapChain);
    ALALBA_ASSERT(err == VK_SUCCESS);

    /// For other private data
    m_imageFormat = surfaceFormat.format;
    
    std::vector<VkImage> swapChainImages;
    //uint32_t imgCount;
    vkGetSwapchainImagesKHR(m_device.Handle(), m_swapChain, &m_imgCount, nullptr);
    swapChainImages.resize(m_imgCount);
    vkGetSwapchainImagesKHR(m_device.Handle(), m_swapChain, &m_imgCount, swapChainImages.data());
    
    for(int i =0;i<swapChainImages.size();i++)
    {
      std::string str_int = std::to_string(i);
      std::string tag = std::string{ "Swapchain" } + str_int;
      m_imageViews.push_back(std::make_unique<ImageView>(m_device, swapChainImages[i], VK_IMAGE_ASPECT_COLOR_BIT, m_imageFormat, VK_IMAGE_VIEW_TYPE_2D, tag));
    }


    // Depthe image and depth image view

	}
	SwapChain::~SwapChain()
	{
    Clean();
	}
  void SwapChain::Clean()
  {
    // clear Image views create by swap chain
    for (int i = 0; i < m_imageViews.size(); i++)
    {
      m_imageViews[i]->Clean();
    }

    if (m_swapChain != VK_NULL_HANDLE)
    {
      ALALBA_WARN("Clean SwapChain {0}", m_tag);
      vkDestroySwapchainKHR(m_device.Handle(), m_swapChain, nullptr);
      m_swapChain = VK_NULL_HANDLE;
    }
  }
}