#pragma once
#include <vulkan/vulkan.h>

namespace vk
{
	class Device;
	class RenderPass;
	class ImageView;

	class FrameBuffer
	{
	public:
		class Builder
		{
		public:
			Builder(const Device& device, const RenderPass& renderpass):m_device(device),m_renderpass(renderpass){};
			Builder& PushAttachment(const ImageView& imageView);
			Builder& SetTag(const std::string& tag) { m_tag = tag; return *this; }
			Builder& SetWidthHeight(const uint32_t width, const uint32_t height) { m_width = width; m_height = height; return *this; }
			std::unique_ptr<FrameBuffer>Build() const 
			{
				return std::make_unique<FrameBuffer>(m_device, m_renderpass, m_pImageViews, m_width, m_height,m_tag);
			};
		private:
			std::vector<const ImageView* > m_pImageViews{};
			const class Device& m_device;
			const class RenderPass& m_renderpass;
			uint32_t m_width;
			uint32_t m_height;

			std::string m_tag{"framebuffer"};
		};

	public:
		VULKAN_NON_COPIABLE(FrameBuffer);
		FrameBuffer(const Device& device, const RenderPass& renderPass, const std::vector<const ImageView*> attachments,
			uint32_t width, uint32_t height, const std::string& tag );
		~FrameBuffer() { Clean(); };
		void Clean();
		const std::vector<const ImageView* >& GetAttachments()const { return m_pAttachments; }

		const VkExtent2D& GetDim()const { return VkExtent2D{ m_width, m_height }; }

	private:
		VULKAN_HANDLE(VkFramebuffer, m_framebuffer);
		const class Device& m_device;
		uint32_t m_width;
		uint32_t m_height;
		std::vector<const ImageView* > m_pAttachments{};
	};

	typedef std::unique_ptr<FrameBuffer> FrameBufferPtr;
}

