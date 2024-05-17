#pragma once
#include <vulkan/vulkan.h>
namespace vk
{
	class Device;
	class DescriptorSetLayout;

	class PipelineLayout final
	{
	public:
		class Builder
		{
		public:
			Builder(const Device& device) :m_device(device) {};
			Builder& SetTag(const std::string& tag) { m_tag = tag; return *this; }
		
			Builder& AddPushConstant(uint32_t size)
			{
				push_constants.push_back({ VK_SHADER_STAGE_VERTEX_BIT , 0, size });
				return *this;
			}
			Builder& BindDescriptorSetLayout(const DescriptorSetLayout& descSetLayout)
			{
				m_descSetLayouts.push_back(&descSetLayout);
				return *this;
			};

			std::unique_ptr<PipelineLayout>Build()const
			{
				return(std::make_unique<PipelineLayout>(m_device, m_descSetLayouts, push_constants, m_tag));
			}

		private:
			std::vector<VkPushConstantRange> push_constants{};
			const Device& m_device;
			std::string m_tag{ "" };
			std::vector<const DescriptorSetLayout*> m_descSetLayouts{};
		};

	public:
		VULKAN_NON_COPIABLE(PipelineLayout);
		PipelineLayout(const Device& device, const std::vector<const DescriptorSetLayout*>& pdescSetLayout,
			const std::vector<VkPushConstantRange>& push_constant,
			const std::string& tag);

		~PipelineLayout() { Clean(); }
		void Clean();
	private:
		VULKAN_HANDLE(VkPipelineLayout, m_pipelineLayout);
		const class Device& m_device;
	};
	typedef std::unique_ptr<PipelineLayout> PipelineLayoutPtr;
}
