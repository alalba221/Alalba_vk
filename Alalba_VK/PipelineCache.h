#pragma once
#include <vulkan/vulkan.h>
namespace vk
{ 
	class Device;

	class PipelineCache
	{
	public:
		class Builder
		{
		public:
			Builder(const Device& device) : m_device(device) {};
			Builder& SetTag(const std::string tag) { m_tag = tag; return *this; }

		private:
			const class Device& m_device;
			std::string m_tag;
		};
		

	public:
		VULKAN_NON_COPIABLE(PipelineCache);

		PipelineCache(const Device& device, const std::string& tag);


	private:
		VULKAN_HANDLE(VkPipelineCache, m_pipelineCache);
		const class Device& m_device;
	};

}

