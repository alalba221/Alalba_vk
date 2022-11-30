#include "pch.h"
#include "Texture.h"

#include "Alalba_VK/Vulkan/Image.h"
#include "Alalba_VK/Vulkan/ImageView.h"
#include "Alalba_VK/Vulkan/Allocator.h"
#include "Alalba_VK/Core/Application.h"

namespace Alalba
{
	vk::Allocator* Texture::s_allocator = nullptr;

	Texture::Texture(const std::string& filename)
		:m_filePath(filename)
	{
		ALALBA_INFO("loading texture image from {0}", m_filePath);
		if (s_allocator == nullptr)
			s_allocator = new vk::Allocator(Application::Get().GetDevice(), Alalba::Application::Get().GetVulkanInstance(), "Texture Allocator");

		// loading an image
		int width, height, channels;
		m_imageData = stbi_load(m_filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

		VkDeviceSize  imageSize = width * height * channels;

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
			Application::Get().GetDevice().GetGraphicsQ(), Application::Get().GetRenderer().GetCommandPool());

	}

	void Texture::Clean()
	{
		m_image->Clean();
	}
}