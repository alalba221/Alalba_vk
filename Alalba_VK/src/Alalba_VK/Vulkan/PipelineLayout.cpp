#include "pch.h"
#include "PipelineLayout.h"
#include "Device.h"
#include "DescriptorSetLayout.h"
// pipeline layout  = descriptor set layout + push constant
namespace vk
{
	PipelineLayout::PipelineLayout(const Device& device,const std::vector<const DescriptorSetLayout*>&pdescSetLayout, 
		const std::vector<VkPushConstantRange>& push_constants, const std::string& tag)
		:m_device(device),m_tag(tag)
	{
		LOG_INFO("Create PipelineLayout: {0}",m_tag);
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		for (int i = 0; i < pdescSetLayout.size(); i++)
		{
			descriptorSetLayouts.push_back(pdescSetLayout[i]->Handle());
		}

		VkPipelineLayoutCreateInfo ci{};
		ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		ci.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		ci.pSetLayouts = descriptorSetLayouts.data();
		ci.pushConstantRangeCount = push_constants.size();
		ci.pPushConstantRanges = push_constants.size()==0?nullptr : push_constants.data();
		ci.pNext = nullptr;
		// ci.flags

		VkResult err;
		err = vkCreatePipelineLayout(m_device.Handle(), &ci, nullptr, &m_pipelineLayout);
		ALALBA_ASSERT(err == VK_SUCCESS, "Create Pipeline Layout failed");
	}

	void PipelineLayout::Clean()
	{
		if (m_pipelineLayout != VK_NULL_HANDLE)
		{
			LOG_WARN("Clean PipelineLayout: {0}", m_tag);
			vkDestroyPipelineLayout(m_device.Handle(), m_pipelineLayout, nullptr);
			m_pipelineLayout = VK_NULL_HANDLE;
		}
	}
}