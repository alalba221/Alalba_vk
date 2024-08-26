#include "pch.h"
#include "FrameBuffer.h"
#include "Device.h"
#include "RenderPass.h"
#include "ImageView.h"

namespace vk
{
	FrameBuffer::Builder& FrameBuffer::Builder::PushAttachment(const ImageView& imageView)
	{
		m_pImageViews.push_back(&imageView);
		return *this;
	}

	FrameBuffer::FrameBuffer(const Device& device, const RenderPass& renderPass, const std::vector<const ImageView*> attachments,
		uint32_t width, uint32_t height, const std::string& tag)
		:m_device(device),m_width(width),m_height(height), m_pAttachments(attachments),m_tag(tag)
	{
		LOG_INFO("Create FrameBuffer: {0}", m_tag);
		std::vector<VkImageView> imgVattachments;

		for (auto attch : m_pAttachments)
		{
			imgVattachments.push_back(attch->Handle());
		}
			
		VkFramebufferCreateInfo CI{};
		CI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		CI.pNext = nullptr;
		//VkFramebufferCreateFlags    flags;
		CI.renderPass = renderPass.Handle();
		CI.attachmentCount = imgVattachments.size();
		CI.pAttachments = imgVattachments.data();
		CI.width = m_width;
		CI.height = m_height;
		CI.layers = 1;

		VkResult err;
		err = vkCreateFramebuffer(m_device.Handle(),&CI,nullptr,&m_framebuffer);
		ALALBA_ASSERT(err == VK_SUCCESS,"Create Framebuffer failed");
	}

	void FrameBuffer::Clean()
	{
		if (m_framebuffer != VK_NULL_HANDLE)
		{
			LOG_WARN("Clean FrameBuffer {0}", m_tag);
			vkDestroyFramebuffer(m_device.Handle(), m_framebuffer, nullptr);
			m_framebuffer = VK_NULL_HANDLE;
		}
	}

}