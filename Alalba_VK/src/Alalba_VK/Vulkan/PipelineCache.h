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
			std::unique_ptr<PipelineCache> Build() const 
			{
				return std::make_unique<PipelineCache>(m_device, m_tag);
			}

		private:
			const class Device& m_device;
			std::string m_tag;
		};
		

	public:
		VULKAN_NON_COPIABLE(PipelineCache);

		PipelineCache(const Device& device, const std::string& tag);
		~PipelineCache() { Clean(); };
		void Clean();

	private:
		VULKAN_HANDLE(VkPipelineCache, m_pipelineCache);
		const class Device& m_device;
	};

	typedef std::unique_ptr<PipelineCache> PipelineCachePtr;
}

