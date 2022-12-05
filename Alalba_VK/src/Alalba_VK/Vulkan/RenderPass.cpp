#include "pch.h"
#include "RenderPass.h"
#include "Device.h"
namespace vk
{
	RenderPass::Builder& RenderPass::Builder::SetColorFormat(const VkFormat colorFormat)
	{
		m_colorFormat = colorFormat;
		return *this;
	}

	RenderPass::Builder& RenderPass::Builder::SetDepthFormat(const VkFormat depthFormat)
	{
		m_depthFormat = depthFormat;
		return *this;
	}

	RenderPass::Builder& RenderPass::Builder::SetColorATCHLoadOP(const VkAttachmentLoadOp colorATCHLoadOp)
	{
		m_colorATCHLoadOp = colorATCHLoadOp;
		return *this;
	}

	RenderPass::Builder& RenderPass::Builder::SetDepthATCHLoadOP(const VkAttachmentLoadOp depthATCHLoadOp)
	{
		m_depthATCHLoadOp = depthATCHLoadOp;
		return *this;
	}
	std::unique_ptr<RenderPass> RenderPass::Builder::Build() const
	{
		return std::make_unique<RenderPass>(m_device, m_colorFormat, m_depthFormat, m_colorATCHLoadOp, m_depthATCHLoadOp);
	}


	RenderPass::RenderPass(const Device& device,
		const VkFormat colorForamt, const VkFormat depthFormat,
		const VkAttachmentLoadOp colorATCHLoadOp,
		const VkAttachmentLoadOp depthATCHLoadOp)
		:m_device(device)
	{
		ALALBA_INFO("Create Render Pass");
		/// Attachment description
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = colorForamt;//swapChain.Format();
		//1 sample, we won't be doing MSAA
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		// we Clear when this attachment is loaded
		colorAttachment.loadOp = colorATCHLoadOp;
		// we keep the attachment stored when the renderpass ends
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		//we don't care about stencil
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		//we don't know or care about the starting layout of the attachment
		colorAttachment.initialLayout = colorATCHLoadOp == VK_ATTACHMENT_LOAD_OP_CLEAR ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		//after the renderpass ends, the image has to be on a layout ready for display
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = depthFormat;// depthBuffer.Format();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = depthATCHLoadOp;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = depthATCHLoadOp == VK_ATTACHMENT_LOAD_OP_CLEAR ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		

		//std::array<VkAttachmentDescription, 1> attachments =
		//{
		//	colorAttachment
		//	//depthAttachment
		//};

		/*Now that our main image target is defined, we need to add a subpass that will render into it. 
		This goes right after defining the attachment*/
		/*UNDEFINED -> RenderPass Begins -> Subpass 0 begins (Transition to Attachment Optimal) ->
		Subpass 0 renders -> Subpass 0 ends -> Renderpass Ends (Transitions to Present Source)*/
		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		//subpass.pDepthStencilAttachment =nullptr;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.inputAttachmentCount = 0;				// optional
		subpass.pInputAttachments = nullptr;		// optional
		subpass.preserveAttachmentCount = 0;		// optional
		subpass.pPreserveAttachments = nullptr;	// optional
		subpass.pResolveAttachments = nullptr;	// optional

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> attachments =
		{
			colorAttachment,
			depthAttachment
		};
		VkRenderPassCreateInfo renderPassCI{};
    renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCI.pNext = nullptr;
    // VkRenderPassCreateFlags           flags;
    renderPassCI.attachmentCount = static_cast<uint32_t>(attachments.size());;
    renderPassCI.pAttachments = attachments.data();
    renderPassCI.subpassCount = 1;
    renderPassCI.pSubpasses = &subpass;
    renderPassCI.dependencyCount = 1;
    renderPassCI.pDependencies = &dependency;

		VkResult err;
		err = vkCreateRenderPass(m_device.Handle(), &renderPassCI, nullptr, &m_renderPass);
		ALALBA_ASSERT(err == VK_SUCCESS, "Create Render Pass Failed");

	}

	void RenderPass::Clean()
	{
		if (m_renderPass != VK_NULL_HANDLE)
		{
			ALALBA_WARN("Clean Render Pass {0}", m_tag);
			vkDestroyRenderPass(m_device.Handle(), m_renderPass, nullptr);
			m_renderPass = VK_NULL_HANDLE;
		}
	}

}

