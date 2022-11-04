#pragma once
#include <vulkan/vulkan.h>
#include "ImageView.h"
#include "Image.h"
#include "FrameBuffer.h"

namespace vk
{
	class Device;
	class Surface;

	class SwapChain final
	{
	public:
		class Builder
		{
		public:
			Builder(const Device& device, const Surface& surface)
				:m_device(device), m_surface(surface) {};
			
			Builder& SetPresentMode(const VkPresentModeKHR& presentMode);
			Builder& SetImgSharingMode(const VkSharingMode& shareMode);
			std::unique_ptr<SwapChain> Build();

		private:
			const class Device& m_device;
			const class Surface& m_surface;
			VkPresentModeKHR m_presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			VkSharingMode m_imageShareMode = VK_SHARING_MODE_EXCLUSIVE;
		};

	public:
		VULKAN_NON_COPIABLE(SwapChain);
		SwapChain(const Device& device, const Surface& surface,
			const VkPresentModeKHR presentMode,
			const VkSharingMode imageShareMode);
		~SwapChain();
		void Clean();
		
		const VkExtent2D& GetExtent()const { return m_extent; }
		const VkFormat& GetFormat() const { return m_imageFormat; }
		const ImageView& GetImageView(const uint32_t index) const;
		const uint32_t GetImgCount() const { return m_imgCount; }
	private:
		VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
		
		
		
		//const std::vector<ImageView> GetImageViews() const;

	private:
		VULKAN_HANDLE(VkSwapchainKHR, m_swapChain);
		const class Device& m_device;
		const class Surface& m_surface;
		
		VkFormat m_imageFormat{};
		VkExtent2D m_extent{};
		std::vector< std::unique_ptr<ImageView> > m_imageViews{};
		uint32_t m_imgCount = 0;

		std::unique_ptr<Image> m_depthImage;
		std::unique_ptr<ImageView> m_depthImageView;
	};
}
