#include "pch.h"
#include "PipelineLayout.h"
#include "Device.h"
namespace vk
{
	PipelineLayout::Builder& PipelineLayout::Builder::AddPushConstant()
	{
		// TODO: insert return statement here
		return *this;
	}
	std::unique_ptr<PipelineLayout> PipelineLayout::Builder::Build()const
	{
		return(std::make_unique<PipelineLayout>(m_device));
	}

	PipelineLayout::PipelineLayout(const Device& device)
		:m_device(device)
	{
		ALALBA_INFO("Create PipelineLayout");
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		for (int i = 0; i < m_device.GetSetLayouCount(); i++)
		{
			descriptorSetLayouts.push_back(m_device.GetDescriptorSetLayout(i).Handle());
		}

		VkPipelineLayoutCreateInfo ci{};
		ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		ci.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		ci.pSetLayouts = descriptorSetLayouts.data();
		ci.pushConstantRangeCount = 0;
		ci.pPushConstantRanges = nullptr; 
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
			ALALBA_WARN("Clean PipelineLayout {0}", m_tag);
			vkDestroyPipelineLayout(m_device.Handle(), m_pipelineLayout, nullptr);
			m_pipelineLayout = VK_NULL_HANDLE;
		}
	}
}