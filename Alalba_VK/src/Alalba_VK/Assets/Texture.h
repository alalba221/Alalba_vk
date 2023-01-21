#pragma once
#include"stb_image.h"

// Need to include sampler.h here, otherwise the sandbox won't be compiled
// If want to use unique_ptr, the compiler need to know the full definition
// https://stackoverflow.com/questions/6012157/is-stdunique-ptrt-required-to-know-the-full-definition-of-t
// TODO:  All the sperate system using unique pointers should include the header function instead of forward declaration
#include "Alalba_VK/Vulkan/Sampler.h"

namespace vk
{
	class Image;
	class ImageView;
//	class Sampler;
	class Allocator;
	class CommandPool;
}

namespace Alalba
{
	class Texture final
	{
	public:
		Texture& operator = (const Texture&) = delete;
		Texture& operator = (Texture&&) = delete;

		Texture(const std::string& filename);
		Texture(uint32_t height, uint32_t width, VkFormat format);

		Texture(const Texture&) = default;
		Texture(Texture&&) = default;
		~Texture() { };
	
		static vk::Allocator* Allocator() { return s_allocator; }
		static vk::CommandPool* CommandPool() { return s_commandPool; }
		static vk::CommandPool* ComputeCommandPool() { return s_computeCmdPool; }

		void Clean();

		const vk::Image& GetImage() const { return *m_image.get(); }
		const vk::ImageView& GetImageView() const { return *m_imageView.get(); }
		const vk::Sampler& GetSampler() const { return *m_sampler.get(); }


	private:
		std::unique_ptr<vk::Image> m_image;
		std::unique_ptr<vk::ImageView> m_imageView;
		std::unique_ptr<vk::Sampler> m_sampler;
		//vk::Sampler* p_sampler;
		unsigned char* m_imageData;
		std::string m_filePath;

		// Allocator for Image
		static vk::Allocator* s_allocator;
		static vk::CommandPool* s_commandPool;
		static vk::CommandPool* s_computeCmdPool;

	};

}


