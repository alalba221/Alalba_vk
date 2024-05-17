#include "pch.h"
#include "ComputePipeline.h"
#include "Device.h"
#include "PipelineLayout.h"
#include "ShaderModule.h"
#include "PipelineCache.h"

namespace vk
{
	ComputePipeline::ComputePipeline(const Device& device, const PipelineLayout& layout, const PipelineCache& pipelineCache, const ShaderModule& computeShader, const std::string tag)
		:m_device(device), m_layout(layout), m_pipelineCache(pipelineCache), m_computeShader(computeShader), m_tag(tag)
	{
		ALALBA_INFO("Create Compute Pipeline: {0}", m_tag);
		 
		VkComputePipelineCreateInfo CI{};
		CI.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		CI.pNext = nullptr;
		// CI.flags;
		CI.stage = m_computeShader.GetStageCI();
		CI.layout = m_layout.Handle();
		//CI.basePipelineHandle;
		//CI.basePipelineIndex;

		VkResult err;
		err = vkCreateComputePipelines(m_device.Handle(), m_pipelineCache.Handle(), 1, &CI, nullptr, &m_pipeline);
		ALALBA_ASSERT(err == VK_SUCCESS, "Create Compute Pipeline failed");
	}
	void ComputePipeline::Clean()
	{
		if (m_pipeline != VK_NULL_HANDLE)
		{
			ALALBA_WARN("Clean Compute Pipeline: {0}", m_tag);
			vkDestroyPipeline(m_device.Handle(), m_pipeline, nullptr);
			m_pipeline = VK_NULL_HANDLE;
		}
	}
}
