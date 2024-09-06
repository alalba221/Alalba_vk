#pragma once
#include <vulkan/vulkan.h>
#include "VkCommon.h"
namespace vk
{
	class Device;
	enum class  AttachmentType
	{
		Color,
		Depth,
		Input,
		Preserve,
		Resolve
	};
	class SubPass
	{
	public:
		SubPass() {};
		~SubPass() {};
		
		SubPass& UseAttachment(uint32_t attachIdx, VkImageLayout layout, AttachmentType type);
		SubPass& SetDescription(bool msaa);

		VkSubpassDescription GetDescrition()const { return m_subPassDesc;}
	private:

		std::vector<VkAttachmentReference> m_colorAttachmentRefs;
		std::vector<VkAttachmentReference> m_resolvedAttachmentRefs;
		std::vector<VkAttachmentReference> m_inputAttachmentRefs;
		VkAttachmentReference m_depthAttachmentRef{};
		std::vector<uint32_t> m_preservedAttachmentRefs;

		VkSubpassDescription m_subPassDesc{};
		bool m_msaa = false;
	};



	class RenderPass
	{
	public: 
		class Builder 
		{
		public:
			Builder(const Device& device):m_device(device) {};
			
			Builder& AddAttachment(VkFormat format, VkSampleCountFlagBits samples, 
				VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
				VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp,
				VkImageLayout initialLayout, VkImageLayout finalLayout);
			
			Builder& AddSubPass(SubPass subpass);
			Builder& SetSubPassDependencies(uint32_t src, VkPipelineStageFlags srcStage, VkAccessFlags srcOp, 
				uint32_t dst, VkPipelineStageFlags dstStage, VkAccessFlags dstOp);

			// for now: dependecy is only for attachment referred images' layout transition
			Builder& SetTag(const std::string tag) { m_tag = tag; return *this; }
			
			std::unique_ptr<RenderPass> Build() const
			{
				return std::make_unique<RenderPass>(m_device, m_attachments,
					m_subPasses, m_dependencies, m_tag);
			}

		private:
			const class Device& m_device;
		
			std::string m_tag;
			std::vector<VkAttachmentDescription> m_attachments{};
			std::vector<SubPass> m_subPasses;


			std::vector<VkSubpassDependency> m_dependencies{};
		};

	public:
		VULKAN_NON_COPIABLE(RenderPass);
		RenderPass(const Device& device, 
			const std::vector<VkAttachmentDescription>& attachments,
			const std::vector<SubPass>& subPass,
			const std::vector<VkSubpassDependency>& dependencies,
			const std::string tag
		);
		~RenderPass() { Clean(); };
		void Clean();


	private:
		VULKAN_HANDLE(VkRenderPass, m_renderPass);
		const class Device& m_device;
		
		std::vector<VkAttachmentDescription> m_attachments{};
		std::vector<SubPass> m_subPasses;
		std::vector<VkSubpassDependency> m_dependencies{};

	};
	typedef std::unique_ptr<RenderPass> RenderPassPtr;
}
