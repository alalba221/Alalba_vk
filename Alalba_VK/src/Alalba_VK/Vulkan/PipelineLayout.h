#pragma once
#include <vulkan/vulkan.h>
namespace vk
{
	class Device;
	
	class PipelineLayout final
	{
	public:
		class Builder
		{
		public:
			Builder(const Device& device) :m_device(device) {};
			Builder& AddPushConstant();
			std::unique_ptr<PipelineLayout>Build()const;

		private:
			const class Device& m_device;
		};

	public:
		VULKAN_NON_COPIABLE(PipelineLayout);
		PipelineLayout(const Device& device);
		~PipelineLayout() { Clean(); }
		void Clean();
	private:
		VULKAN_HANDLE(VkPipelineLayout, m_pipelineLayout);
		const class Device& m_device;
	};
}
