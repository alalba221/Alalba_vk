#include "pch.h"
#include "CubeTexture.h"

#include "Alalba_VK/Core/Application.h"
//#include "Alalba_VK/Core/Scene.h"

namespace Alalba
{
	CubeTexture::CubeTexture(const Scene& scene, const std::string& filename)
		:m_filePath(filename)
	{
		//ALALBA_INFO("loading Cube texture image from {0}", m_filePath);

		//// loading an image
		//int width, height, channels;
		//m_imageData = stbi_load(m_filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

		//VkDeviceSize  imageSize = width * height * 4;
		//// Create Image Object on GPU
		//m_image = vk::Image::Builder(Application::Get().GetDevice(), scene.GetAllocator())
		//	.SetTag("Texture Image Object")
		//	.SetImgType(VK_IMAGE_TYPE_2D)
		//	.SetImgExtent({ static_cast<uint32_t>(width) ,static_cast<uint32_t>(height),1 })
		//	.SetImageFormat(VK_FORMAT_R8G8B8A8_SRGB)
		//	.SetUsageFlags(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
		//	.SetImageTiling(VK_IMAGE_TILING_OPTIMAL)
		//	.Build();

		//m_image->CopyImageFrom(m_imageData, imageSize,
		//	Application::Get().GetDevice().GetGraphicsQ(), scene.GetCommandPool());
		//stbi_image_free(m_imageData);

		//// image view
		//m_imageView = vk::ImageView::Builder(Application::Get().GetDevice(), *m_image)
		//	.SetFormat(VK_FORMAT_R8G8B8A8_SRGB)
		//	.SetTag("Texture ImageView")
		//	.SetSubresourceAspectFlags(VK_IMAGE_ASPECT_COLOR_BIT)
		//	.SetViewType(VK_IMAGE_VIEW_TYPE_2D)
		//	.Build();

		//// sampler
		//m_sampler = vk::Sampler::Builder(Application::Get().GetDevice())
		//	.SetTag("Texture Sampler")
		//	.SetFilter(VK_FILTER_LINEAR)
		//	.SetMipmapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR)
		//	.SetAddressMode(VK_SAMPLER_ADDRESS_MODE_REPEAT)
		//	.Build();
	}
}
