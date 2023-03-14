#include "pch.h"
#include "PipelineCache.h"
#include "Device.h"

namespace vk
{
	PipelineCache::PipelineCache(const Device& device, const std::string& tag)
		:m_device(device),m_tag(tag)
	{
		ALALBA_INFO("Create PipelineCache: {0}", m_tag);

		VkPipelineCacheCreateInfo CI{};
		CI.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		//CI.pNext = nullptr;
		//CI.flags;
		//CI.initialDataSize;
		//CI.pInitialData;

		VkResult err;
		err = vkCreatePipelineCache(m_device.Handle(), &CI, nullptr, &m_pipelineCache);
		ALALBA_ASSERT(err == VK_SUCCESS, "Create Pipeline Cache failed");
	}

	void PipelineCache::Clean()
	{
		if (m_pipelineCache != VK_NULL_HANDLE)
		{
			ALALBA_WARN("Clean Pipeline Cache: {0}", m_tag);
			vkDestroyPipelineCache(m_device.Handle(), m_pipelineCache, nullptr);
			m_pipelineCache = VK_NULL_HANDLE;
		}
	}

}

