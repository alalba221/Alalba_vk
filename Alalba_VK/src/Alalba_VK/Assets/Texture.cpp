#include "pch.h"
#include "Texture.h"

#include "Alalba_VK/Vulkan/Image.h"
#include "Alalba_VK/Vulkan/ImageView.h"
namespace vk
{
	Texture::Texture(const std::string& filename)
		:m_filePath(filename)
	{
		ALALBA_INFO("loading texture image from {0}", m_filePath);
		int width, height, channels;
		m_imageData = stbi_load(m_filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);


	}
}