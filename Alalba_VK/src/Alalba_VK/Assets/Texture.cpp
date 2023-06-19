#include "pch.h"
#include "Texture.h"
#include "TextureSys.h"

#include "Alalba_VK/Core/Application.h"

//#include "Alalba_VK/Core/Scene.h"
//#include "Alalba_VK/Core/VulkanComputer.h"

namespace Alalba
{

	vk::CommandPool* Texture::s_computeCmdPool = nullptr;

	Texture::Texture(VkFormat format,const std::string& filename, vk::Allocator& allocator, vk::CommandPool& cmdPool)
		:m_filePath(filename)
	{
		ALALBA_INFO("loading texture image from {0}", m_filePath);

		// loading an image
		int width, height, channels;
		unsigned char* imageData = stbi_load(m_filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

		Application& app = Application::Get();

		VkDeviceSize  imageSize = width * height * 4;
		// Create Image Object on GPU
		m_image = vk::Image::Builder(app.GetDevice(), allocator)
			.SetTag("Texture Image Object")
			.SetImgType(VK_IMAGE_TYPE_2D)
			.SetImgExtent({ static_cast<uint32_t>(width) ,static_cast<uint32_t>(height),1 })
			.SetImageFormat(format)
			.SetUsageFlags(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
			.SetImageTiling(VK_IMAGE_TILING_OPTIMAL)
			.SetSharingMode(VK_SHARING_MODE_EXCLUSIVE)
			.Build();

		m_image->CopyTextureImageFrom(imageData, imageSize,
			app.GetDevice().GetGraphicsQ(), cmdPool);
		stbi_image_free(imageData);

		// image view
		m_imageView = vk::ImageView::Builder(app.GetDevice(), *m_image)
			.SetFormat(format)
			.SetTag("Texture ImageView")
			.SetSubresourceAspectFlags(VK_IMAGE_ASPECT_COLOR_BIT)
			.SetViewType(VK_IMAGE_VIEW_TYPE_2D)
			.Build();

		// sampler
		m_sampler = vk::Sampler::Builder(app.GetDevice())
			.SetTag("Texture Sampler")
			.SetFilter(VK_FILTER_LINEAR)
			.SetMipmapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR)
			.SetAddressMode(VK_SAMPLER_ADDRESS_MODE_REPEAT)
			.Build();
	}

	// Build from Buffer
	Texture::Texture(void* buffer, VkDeviceSize bufferSize, VkFormat format, uint32_t texWidth, uint32_t texHeight, vk::Allocator& allocator, vk::CommandPool& cmdPool)
	{
		const vk::Device& device = Application::Get().GetDevice();

		m_image = vk::Image::Builder(device, allocator)
			.SetTag("Texture Image Object")
			.SetImgType(VK_IMAGE_TYPE_2D)
			.SetImgExtent({ static_cast<uint32_t>(texWidth) ,static_cast<uint32_t>(texHeight),1 })
			.SetImageFormat(format)
			.SetUsageFlags(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
			.SetImageTiling(VK_IMAGE_TILING_OPTIMAL)
			.SetSharingMode(VK_SHARING_MODE_EXCLUSIVE)
			.Build();

		m_image->CopyTextureImageFrom(buffer, bufferSize,
			device.GetGraphicsQ(), cmdPool);

		// image view
		m_imageView = vk::ImageView::Builder(device, *m_image)
			.SetFormat(format)
			.SetTag("Texture ImageView")
			.SetSubresourceAspectFlags(VK_IMAGE_ASPECT_COLOR_BIT)
			.SetViewType(VK_IMAGE_VIEW_TYPE_2D)
			.Build();

		// sampler
		m_sampler = vk::Sampler::Builder(device)
			.SetTag("Texture Sampler")
			.SetFilter(VK_FILTER_LINEAR)
			.SetMipmapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR)
			.SetAddressMode(VK_SAMPLER_ADDRESS_MODE_REPEAT)
			.Build();
	}


	////TODO: Used to create a target texture for compute shader
	//Texture::Texture(const vk::VulkanComputer& computer, uint32_t height, uint32_t width, VkFormat format)
	//{
	//	// static member
	//	ALALBA_INFO("Creating Target texture");

	//	//
	//	VkDeviceSize  imageSize = width * height * 4;
	//	// Create Image Object on GPU
	//	m_image = vk::Image::Builder(Application::Get().GetDevice(), computer.GetAllocator())
	//		.SetTag("Target Texture Image Object")
	//		.SetImgType(VK_IMAGE_TYPE_2D)
	//		.SetImgExtent({ static_cast<uint32_t>(width) ,static_cast<uint32_t>(height),1 })
	//		.SetImageFormat(format)
	//		.SetUsageFlags(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT)
	//		.SetImageTiling(VK_IMAGE_TILING_OPTIMAL)
	//		.SetSharingMode(VK_SHARING_MODE_CONCURRENT)
	//		.Build();
	//	// Image layout transit 
	//	m_image->TransitionImageLayout(computer.GetCommandPool(), Application::Get().GetDevice().GetComputeQ(), VK_IMAGE_LAYOUT_GENERAL);

	//	// image view
	//	m_imageView = vk::ImageView::Builder(Application::Get().GetDevice(), *m_image)
	//		.SetFormat(format)
	//		.SetTag("Target Texture ImageView")
	//		.SetSubresourceAspectFlags(VK_IMAGE_ASPECT_COLOR_BIT)
	//		.SetViewType(VK_IMAGE_VIEW_TYPE_2D)
	//		.Build();

	//	// sampler
	//	m_sampler = vk::Sampler::Builder(Application::Get().GetDevice())
	//		.SetTag("Target Texture Sampler")
	//		.SetFilter(VK_FILTER_LINEAR)
	//		.SetMipmapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR)
	//		.SetAddressMode(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER)
	//		.Build();
	//}

	void Texture::Clean()
	{
		m_image->Clean();
		m_imageView->Clean();
		m_sampler->Clean();
	}
}