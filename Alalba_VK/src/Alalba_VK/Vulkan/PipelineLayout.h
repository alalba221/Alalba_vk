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
			Builder& AddPushConstant();
			Builder& BindDescriptorSetLayout(const DescriptorSetLayout& descSetLayout)
			{
				m_descSetLayouts.push_back(&descSetLayout);
				return *this;
			};

			std::unique_ptr<PipelineLayout>Build()const
			{
				return(std::make_unique<PipelineLayout>(m_device, m_descSetLayouts, m_tag));
			}

		private:
			const Device& m_device;
			std::string m_tag{ "" };
			std::vector<const DescriptorSetLayout*> m_descSetLayouts{};
		};

	public:
		VULKAN_NON_COPIABLE(PipelineLayout);
		PipelineLayout(const Device& device, const std::vector<const DescriptorSetLayout*>pdescSetLayout,
			const std::string& tag);

		~PipelineLayout() { Clean(); }
		void Clean();
	private:
		VULKAN_HANDLE(VkPipelineLayout, m_pipelineLayout);
		const class Device& m_device;
	};
}
