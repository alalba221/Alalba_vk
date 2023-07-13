#pragma once
#include <vulkan/vulkan.h>

namespace vk
{
	class Device;
	class RenderPass
	{
	public: 
		class Builder 
		{
		public:
			Builder(const Device& device):m_device(device) {};
			
			Builder& PushColorAttachment(VkFormat format, VkAttachmentLoadOp loadop, VkImageLayout initialLayout, VkImageLayout finalLayout);
			Builder& PushDepthAttachment(VkFormat format, VkAttachmentLoadOp loadop, VkImageLayout initialLayout, VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE, VkImageLayout finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
			// for now: dependecy is only for attachment referred images' layout transition
			Builder& AddDependency(uint32_t src, VkPipelineStageFlags srcStage, VkAccessFlags srcOp, uint32_t dst, VkPipelineStageFlags dstStage, VkAccessFlags dstOp);
			Builder& SetTag(const std::string tag) { m_tag = tag; return *this; }
			std::unique_ptr<RenderPass> Build() const
			{
				return std::make_unique<RenderPass>(m_device, m_attachments, m_colorAttachmentRefs, m_depthAttachmentRef, m_dependencies, m_tag);
			}

		private:
			const class Device& m_device;
		
			std::string m_tag;
			uint32_t nextColorAttachIndex = 0;

			std::vector<VkAttachmentDescription> m_attachments{};
			std::vector<VkAttachmentReference> m_colorAttachmentRefs{};
			VkAttachmentReference m_depthAttachmentRef{};

			std::vector<VkSubpassDependency> m_dependencies{};
		};

	public:
		VULKAN_NON_COPIABLE(RenderPass);
		RenderPass(const Device& device, 
			const std::vector<VkAttachmentDescription>& attachments,
			const std::vector<VkAttachmentReference>& colorAttachmentRefs, const VkAttachmentReference& depthAttachmentRef,
			const std::vector<VkSubpassDependency>& dependencies,
			const std::string tag
		);
		~RenderPass() { Clean(); };
		void Clean();

		const uint32_t ColorAttachmentCount()const { return m_colorAttachmentCount; }

	private:
		VULKAN_HANDLE(VkRenderPass, m_renderPass);
		const class Device& m_device;

		uint32_t m_colorAttachmentCount;
	};
}
