#pragma once
#include <vulkan/vulkan.h>
namespace vk
{
	class Device;
	class ShaderModule;
	class PipelineLayout;

	class ComputePipeline
	{
	public:
		class Builder
		{
		public:
			Builder(const Device& device, const PipelineLayout& layout,
				const ShaderModule& computeShader)
				:m_device(device),m_layout(layout),m_computeShader(computeShader)
			{};

			Builder& SetTag(const std::string tag) { m_tag = tag; return *this; }

			std::unique_ptr<ComputePipeline> Build() const
			{
				return std::make_unique<ComputePipeline>(m_device, m_layout, m_computeShader, m_tag);
			}
		private:
			const class Device& m_device;
			const class PipelineLayout& m_layout;
			const class ShaderModule& m_computeShader;
			std::string m_tag;
		};

	public:
		VULKAN_NON_COPIABLE(ComputePipeline);

		ComputePipeline(const Device& device, const PipelineLayout& layout,
			const ShaderModule& computeShader, const std::string tag);

		~ComputePipeline() { Clean(); };

		void Clean();
	private:
		VULKAN_HANDLE(VkPipeline, m_pipeline);
		const class Device& m_device;
		const class PipelineLayout& m_layout;
		const class ShaderModule& m_computeShader;

	};
}


