#pragma once
#include"stb_image.h"

// Need to include sampler.h here, otherwise the sandbox won't be compiled
// If want to use unique_ptr, the compiler need to know the full definition
// https://stackoverflow.com/questions/6012157/is-stdunique-ptrt-required-to-know-the-full-definition-of-t
// TODO:  All the sperate system using unique pointers should include the header function instead of forward declaration
#include "Alalba_VK/Vulkan/Sampler.h"
#include "Alalba_VK/Vulkan/Image.h"
#include "Alalba_VK/Vulkan/ImageView.h"
namespace vk
{
	class Image;
	class ImageView;
	//	class Sampler;
	class VulkanComputer;
}

namespace Alalba
{
	class	Scene;
	class CubeTexture final
	{
	public:

		CubeTexture& operator = (const CubeTexture&) = delete;
		CubeTexture& operator = (CubeTexture&&) = delete;

		CubeTexture(const Scene& scene, const std::string& filename);
		
		CubeTexture(const CubeTexture&) = default;
		CubeTexture(CubeTexture&&) = default;
		~CubeTexture() { };

		void Clean();

		const vk::Image& GetImage() const { return *m_image; }
		const vk::ImageView& GetImageView() const { return *m_imageView; }
		const vk::Sampler& GetSampler() const { return *m_sampler; }

	private:
		std::unique_ptr<vk::Image> m_image;
		std::unique_ptr<vk::ImageView> m_imageView;
		std::unique_ptr<vk::Sampler> m_sampler;
		//vk::Sampler* p_sampler;
		unsigned char* m_imageData;
		std::string m_filePath;


	};

}




