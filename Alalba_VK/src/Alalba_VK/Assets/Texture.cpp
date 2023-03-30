#include "pch.h"
#include "Texture.h"

#include "Alalba_VK/Vulkan/CommandPool.h"
#include "Alalba_VK/Vulkan/Allocator.h"
#include "Alalba_VK/Core/Application.h"

namespace Alalba
{
	vk::Allocator* Texture::s_allocator = nullptr;
	vk::CommandPool* Texture::s_commandPool = nullptr;
	vk::CommandPool* Texture::s_computeCmdPool = nullptr;

	Texture::Texture(const std::string& filename)
		:m_filePath(filename)
	{
		ALALBA_INFO("loading texture image from {0}", m_filePath);
		if (s_allocator == nullptr)
			s_allocator = new vk::Allocator(Application::Get().GetDevice(), Alalba::Application::Get().GetVulkanInstance(), "Texture Allocator");
		
		if (s_commandPool == nullptr)
			s_commandPool = new vk::CommandPool(Application::Get().GetDevice(), 
				Alalba::Application::Get().GetVulkanInstance().GetPhysicalDevice().GetQFamilies().graphics.value(), 
				VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
				"Texture CommandPool");

		// loading an image
		int width, height, channels;
		m_imageData = stbi_load(m_filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

		VkDeviceSize  imageSize = width * height * 4;
		// Create Image Object on GPU
		m_image = vk::Image::Builder(Application::Get().GetDevice(), *s_allocator)
			.SetTag("Texture Image Object")
			.SetImgType(VK_IMAGE_TYPE_2D)
			.SetImgExtent({ static_cast<uint32_t>(width) ,static_cast<uint32_t>(height),1})
			.SetImageFormat(VK_FORMAT_R8G8B8A8_SRGB)
			.SetUsageFlags(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
			.SetImageTiling(VK_IMAGE_TILING_OPTIMAL)
			.Build();

		m_image->CopyImageFrom(m_imageData, imageSize,
			Application::Get().GetDevice().GetGraphicsQ(), *s_commandPool);
		stbi_image_free(m_imageData);

		// image view
		m_imageView = vk::ImageView::Builder(Application::Get().GetDevice(), *m_image.get())
			.SetFormat(VK_FORMAT_R8G8B8A8_SRGB)
			.SetTag("Texture ImageView")
			.SetSubresourceAspectFlags(VK_IMAGE_ASPECT_COLOR_BIT)
			.SetViewType(VK_IMAGE_VIEW_TYPE_2D)
			.Build();

	// sampler
		m_sampler = vk::Sampler::Builder(Application::Get().GetDevice())
			.SetTag("Texture Sampler")
			.SetFilter(VK_FILTER_LINEAR)
			.SetMipmapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR)
			.SetAddressMode(VK_SAMPLER_ADDRESS_MODE_REPEAT)
			.Build();
	}

	//TODO: Used to create a target texture for compute shader
	Texture::Texture(uint32_t height, uint32_t width, VkFormat format)
	{
		// static member
		ALALBA_INFO("Creating Target texture");
		if (s_allocator == nullptr)
			s_allocator = new vk::Allocator(Application::Get().GetDevice(), Alalba::Application::Get().GetVulkanInstance(), "Texture Allocator");

		if (s_computeCmdPool == nullptr)
			s_computeCmdPool = new vk::CommandPool(Application::Get().GetDevice(),
				Alalba::Application::Get().GetVulkanInstance().GetPhysicalDevice().GetQFamilies().compute.value(),
				VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
				"Texture CommandPool");
		//
		VkDeviceSize  imageSize = width * height * 4;
		// Create Image Object on GPU
		m_image = vk::Image::Builder(Application::Get().GetDevice(), *s_allocator)
			.SetTag("Target Texture Image Object")
			.SetImgType(VK_IMAGE_TYPE_2D)
			.SetImgExtent({ static_cast<uint32_t>(width) ,static_cast<uint32_t>(height),1 })
			.SetImageFormat(format)
			.SetUsageFlags(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT)
			.SetImageTiling(VK_IMAGE_TILING_OPTIMAL)
			.SetSharingMode(VK_SHARING_MODE_CONCURRENT)
			.Build();
		// Image layout transit 
		m_image->TransitionImageLayout(*s_computeCmdPool, Application::Get().GetDevice().GetComputeQ(), VK_IMAGE_LAYOUT_GENERAL);
		
		// image view
		m_imageView = vk::ImageView::Builder(Application::Get().GetDevice(), *m_image.get())
			.SetFormat(format)
			.SetTag("Target Texture ImageView")
			.SetSubresourceAspectFlags(VK_IMAGE_ASPECT_COLOR_BIT)
			.SetViewType(VK_IMAGE_VIEW_TYPE_2D)
			.Build();

		// sampler
		m_sampler = vk::Sampler::Builder(Application::Get().GetDevice())
			.SetTag("Target Texture Sampler")
			.SetFilter(VK_FILTER_LINEAR)
			.SetMipmapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR)
			.SetAddressMode(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER)
			.Build();
	}

	void Texture::Clean()
	{
		//vkDeviceWaitIdle(Application::Get().GetDevice().Handle());
		m_image->Clean();
		m_imageView->Clean();
		m_sampler->Clean();
		//p_sampler->Clean();
	}
}