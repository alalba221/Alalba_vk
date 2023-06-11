#include "pch.h"
#include "RenderPass.h"
#include "Device.h"
namespace vk
{

	RenderPass::Builder& RenderPass::Builder::PushColorAttachment(VkFormat format, VkAttachmentLoadOp loadop, VkImageLayout initialLayout, VkImageLayout finalLayout)
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = format;//swapChain.Format();
		//1 sample, we won't be doing MSAA
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		// we Clear when this attachment is loaded
		colorAttachment.loadOp = loadop;
		// we keep the attachment stored when the renderpass ends
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		//we don't care about stencil
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		//we don't know or care about the starting layout of the attachment
		colorAttachment.initialLayout = initialLayout;
		//after the renderpass ends, the image has to be on a layout ready for display
		colorAttachment.finalLayout = finalLayout;

		m_attachments.push_back(colorAttachment);

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = nextColorAttachIndex;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		m_colorAttachmentRefs.push_back(colorAttachmentRef);
		
		nextColorAttachIndex++;
		
		ALALBA_ASSERT(nextColorAttachIndex == m_attachments.size(), "Depth attachment shoule be added at the end");
		return *this;
	}

	RenderPass::Builder& RenderPass::Builder::PushDepthAttachment(VkFormat format, VkAttachmentLoadOp loadop, VkImageLayout initialLayout, VkImageLayout finalLayout)
	{
		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = format;// depthBuffer.Format();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = loadop;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = initialLayout;
		depthAttachment.finalLayout = finalLayout;

		m_attachments.push_back(depthAttachment);

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = nextColorAttachIndex;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		m_depthAttachmentRef = depthAttachmentRef;
		
		return *this;
	}

	RenderPass::Builder& RenderPass::Builder::PushDependency(VkPipelineStageFlags srcStage, VkAccessFlags srcOp, VkPipelineStageFlags dstStage, VkAccessFlags dstOp)
	{
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = srcStage;
		dependency.srcAccessMask = srcOp;
		dependency.dstStageMask = dstStage;
		dependency.dstAccessMask = dstOp;

		m_dependencies.push_back(dependency);
		return *this;
	}


	// TODO: should have a add attachment function
	RenderPass::RenderPass(const Device& device,
		const std::vector<VkAttachmentDescription>& attachments,
		const std::vector<VkAttachmentReference>& colorAttachmentRefs, const VkAttachmentReference& depthAttachmentRef,
		const std::vector<VkSubpassDependency>& dependencies,
		const std::string tag)
		:m_device(device), m_tag(tag)
	{
		ALALBA_INFO("Create Render Pass: {0}", m_tag);
	
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = colorAttachmentRefs.size();
		subpass.pColorAttachments = colorAttachmentRefs.data();
		//subpass.pDepthStencilAttachment =nullptr;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.inputAttachmentCount = 0;				// optional
		subpass.pInputAttachments = nullptr;		// optional
		subpass.preserveAttachmentCount = 0;		// optional
		subpass.pPreserveAttachments = nullptr;	// optional
		subpass.pResolveAttachments = nullptr;	// optional

		VkRenderPassCreateInfo renderPassCI{};
    renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCI.pNext = nullptr;
    // VkRenderPassCreateFlags           flags;
    renderPassCI.attachmentCount = static_cast<uint32_t>(attachments.size());;
    renderPassCI.pAttachments = attachments.data();
    renderPassCI.subpassCount = 1;
    renderPassCI.pSubpasses = &subpass;
    renderPassCI.dependencyCount = dependencies.size();
    renderPassCI.pDependencies = dependencies.data();

		VkResult err;
		err = vkCreateRenderPass(m_device.Handle(), &renderPassCI, nullptr, &m_renderPass);
		ALALBA_ASSERT(err == VK_SUCCESS, "Create Render Pass Failed");

		m_colorAttachmentCount = colorAttachmentRefs.size();
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

