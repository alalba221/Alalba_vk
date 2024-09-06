#include "pch.h"
#include "RenderPass.h"
#include "Device.h"
namespace vk
{
	SubPass& SubPass::UseAttachment(uint32_t attachIdx, VkImageLayout layout, AttachmentType type)
	{
		VkAttachmentReference attachRef{};
		attachRef.layout = layout;
		attachRef.attachment = attachIdx;
		
		switch(type)
		{
		case AttachmentType::Color:
			m_colorAttachmentRefs.push_back(attachRef);
			break;
		case AttachmentType::Depth:
			m_depthAttachmentRef = attachRef;
			break;
		case AttachmentType::Input:
			m_inputAttachmentRefs.push_back(attachRef);
			break;
		case AttachmentType::Preserve:
			m_preservedAttachmentRefs.push_back(attachIdx);
			break;
		case AttachmentType::Resolve:
			m_resolvedAttachmentRefs.push_back(attachRef);
			break;

		default:
			LOG_ERROR("Attachment Type not supported");
			break;
		}
		return *this;
	}

	SubPass& SubPass::SetDescription(bool msaa)
	{
		if (msaa)
			m_resolvedAttachmentRefs.resize(static_cast<uint32_t>(m_colorAttachmentRefs.size()));

		m_subPassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		m_subPassDesc.colorAttachmentCount = static_cast<uint32_t>(m_colorAttachmentRefs.size());
		m_subPassDesc.pColorAttachments = m_colorAttachmentRefs.data();
		m_subPassDesc.pDepthStencilAttachment = &m_depthAttachmentRef;
		m_subPassDesc.inputAttachmentCount = static_cast<uint32_t>(m_inputAttachmentRefs.size());
		m_subPassDesc.pInputAttachments = m_inputAttachmentRefs.size() == 0 ? nullptr: m_inputAttachmentRefs.data();
		m_subPassDesc.preserveAttachmentCount = static_cast<uint32_t>(m_preservedAttachmentRefs.size());
		m_subPassDesc.pPreserveAttachments = m_preservedAttachmentRefs.size() == 0 ? nullptr : m_preservedAttachmentRefs.data();
		m_subPassDesc.pResolveAttachments = m_resolvedAttachmentRefs.data();	
		return *this;
	}

	

	RenderPass::Builder& RenderPass::Builder::AddAttachment(VkFormat format, VkSampleCountFlagBits samples,
																						VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
																						VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp,
																						VkImageLayout initialLayout, VkImageLayout finalLayout)
	{
		VkAttachmentDescription Attachment = {};
		Attachment.format = format;//swapChain.Format();
		Attachment.samples = samples;
		Attachment.loadOp = loadOp;
		Attachment.storeOp = storeOp;
		Attachment.stencilLoadOp = stencilLoadOp;
		Attachment.stencilStoreOp = stencilStoreOp;
		Attachment.initialLayout = initialLayout;
		Attachment.finalLayout = finalLayout;
		m_attachments.push_back(Attachment);

		return *this;
	}

	RenderPass::Builder& RenderPass::Builder::AddSubPass(SubPass subpass)
	{
		m_subPasses.push_back(subpass);
		return *this;
	}

	RenderPass::Builder& RenderPass::Builder::SetSubPassDependencies(uint32_t src, VkPipelineStageFlags srcStage, VkAccessFlags srcOp,
				uint32_t dst, VkPipelineStageFlags dstStage, VkAccessFlags dstOp)
	{
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = src;
		dependency.dstSubpass = dst;
		dependency.srcStageMask = srcStage;
		dependency.srcAccessMask = srcOp;
		dependency.dstStageMask = dstStage;
		dependency.dstAccessMask = dstOp;
		dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		m_dependencies.push_back(dependency);
		return *this;
	}



	// TODO: should have an add attachment function
	RenderPass::RenderPass(const Device& device,
		const std::vector<VkAttachmentDescription>& attachments,
		const std::vector<SubPass>& subPass,
		const std::vector<VkSubpassDependency>& dependencies,
		const std::string tag)
		:m_device(device), m_attachments(attachments), m_subPasses(subPass),m_dependencies(dependencies),m_tag(tag)
	{
		LOG_INFO("Create Render Pass: {0}", m_tag);

		std::vector<VkSubpassDescription> subPassDesc;

		for (auto subPass : m_subPasses)
		{
			subPassDesc.push_back(subPass.GetDescrition());
		}

		VkRenderPassCreateInfo renderPassCI{};
    renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCI.pNext = nullptr;
    // VkRenderPassCreateFlags           flags;
    renderPassCI.attachmentCount = static_cast<uint32_t>(m_attachments.size());;
    renderPassCI.pAttachments = m_attachments.data();
    renderPassCI.subpassCount = static_cast<uint32_t>(m_subPasses.size());
    renderPassCI.pSubpasses = subPassDesc.data();
    renderPassCI.dependencyCount = static_cast<uint32_t>(m_dependencies.size());
    renderPassCI.pDependencies = m_dependencies.data();

		VkResult err;
		err = vkCreateRenderPass(m_device.Handle(), &renderPassCI, nullptr, &m_renderPass);
		ALALBA_ASSERT(err == VK_SUCCESS, "Create Render Pass Failed");
		
		LOG_TRACE("RenderPass {0} : {1}, attachment count: {2}, subpass count: {3}", __FUNCTION__, (void*)m_renderPass, m_attachments.size(), m_subPasses.size());
	}

	void RenderPass::Clean()
	{
		if (m_renderPass != VK_NULL_HANDLE)
		{
			//LOG_WARN("Clean Render Pass {0}", m_tag);
			vkDestroyRenderPass(m_device.Handle(), m_renderPass, nullptr);
			m_renderPass = VK_NULL_HANDLE;
		}
	}



}

