#pragma once
#include <vulkan/vulkan.h>
namespace vk
{
	class Device;
	class ShaderModule;
	class PipelineLayout;
	class RenderPass;
	class PipelineCache;

	class GraphicsPipeline
	{
	public:
		class Builder
		{
		public:
			Builder(const Device& device, const PipelineLayout& layout, const RenderPass& renderpass,
				const ShaderModule& vertex, const ShaderModule& fragment, const PipelineCache& pipelineCache)
				:m_device(device), m_layout(layout),m_renderpass(renderpass), m_vertShader(vertex), m_fragShader(fragment),
			m_pipelineCache(pipelineCache){};
				
			Builder& SetAssemblyTopology(VkPrimitiveTopology topology) { m_topology = topology; return *this; };
			Builder& SetPolygonMode(VkPolygonMode polygonMode) { m_polygonMode = polygonMode; return *this; };
			Builder& SetBackCulling(bool backcull) { m_backcull = backcull; return *this; }
			Builder& SetViewPortWidth(float width) { m_viewportWidth = width; return *this; };
			Builder& SetViewPortHeight(float height) { m_viewportHeight = height; return *this; };
			Builder& SetScirrorExtent(VkExtent2D scirrorExtent) { m_scirrorExtent = scirrorExtent; return *this; };
			

			std::unique_ptr<GraphicsPipeline> Build() const
			{
				return std::make_unique<GraphicsPipeline>(m_device, m_layout, m_renderpass,m_vertShader, m_fragShader, m_pipelineCache,
					m_topology, m_polygonMode, m_backcull
					, m_viewportWidth, m_viewportHeight, m_scirrorExtent);
			};

		private:
			const class Device& m_device;
			const class PipelineLayout& m_layout;
			const class RenderPass& m_renderpass;
			const class ShaderModule& m_vertShader;
			const class ShaderModule& m_fragShader;
			const class PipelineCache& m_pipelineCache;

			
			VkPrimitiveTopology m_topology{};
			VkPolygonMode m_polygonMode{};
			float m_viewportWidth;
			float m_viewportHeight;
			VkExtent2D m_scirrorExtent;

			bool m_backcull = true;
		};

	public:
		VULKAN_NON_COPIABLE(GraphicsPipeline);
		GraphicsPipeline(const Device& device, const PipelineLayout& layout, const RenderPass& renderpass,
			const ShaderModule& vertex, const ShaderModule& fragment, const PipelineCache& pipelineCache,
			const VkPrimitiveTopology topology,
			const VkPolygonMode polygonMode,
			const bool backculling,
			const float viewportWidth, const float viewportHight, const VkExtent2D scirrorExtent
			);

		~GraphicsPipeline() { Clean(); };
		void Clean();

	private:
		VULKAN_HANDLE(VkPipeline, m_pipeline);
		const class Device& m_device;
		const class PipelineLayout& m_layout;
		const class RenderPass& m_renderpass;
		const class ShaderModule& m_fragShader;
		const class ShaderModule& m_vertShader;
		const class PipelineCache& m_pipelineCache;

	};
}
