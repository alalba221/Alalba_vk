#include "pch.h"
#include "PipelineCache.h"
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
	}

}

