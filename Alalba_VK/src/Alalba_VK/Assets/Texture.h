#pragma once
#include"stb_image.h"

namespace vk
{
	class Image;
	class ImageView;
	class Sampler;
	class Allocator;
}

namespace Alalba
{
	class Texture final
	{
	public:
		Texture& operator = (const Texture&) = delete;
		Texture& operator = (Texture&&) = delete;

		Texture(const std::string& filename);
		Texture(const Texture&) = default;
		Texture(Texture&&) = default;
		~Texture() { Clean(); };
	
		static vk::Allocator* Allocator() { return s_allocator; }
		void Clean();

	private:
		std::unique_ptr<vk::Image> m_image;
		std::unique_ptr<vk::ImageView> m_imageView;
		std::unique_ptr<vk::Sampler> m_sampler;

		unsigned char* m_imageData;
		std::string m_filePath;

		// Allocator for Image
		static vk::Allocator* s_allocator;
	};

}


