#pragma once

#include"stb_image.h"

namespace vk
{
	class Image;
	class ImageView;

	class Texture final
	{
	public:
		Texture& operator = (const Texture&) = delete;
		Texture& operator = (Texture&&) = delete;

		Texture(const std::string& filename);
		Texture(const Texture&) = default;
		Texture(Texture&&) = default;
		~Texture() = default;
	
	private:
		std::unique_ptr<Image> m_image;
		std::unique_ptr<ImageView> m_imageView;

		unsigned char* m_imageData;
		std::string m_filePath;
	};

}


