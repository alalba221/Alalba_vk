#pragma once
#include <vulkan/vulkan.h>
#include "VkCommon.h"
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
				 const PipelineCache& pipelineCache)
				:m_device(device), m_layout(layout),m_renderpass(renderpass), m_pipelineCache(pipelineCache)
			{};
				

			std::unique_ptr<GraphicsPipeline> Build() const
			{
				return std::make_unique<GraphicsPipeline>(m_device, m_layout, 
					m_renderpass, m_pipelineCache,
					m_shaderStages,
					m_vertexInputState,
					m_inputAssembly,
					m_rasterizerState,
					m_depthState,
					m_multisamplingState,
					m_colorBlendState,
					m_viewportState,
					m_dynamicState,
					m_tag);
			};

			Builder& SetTag(const std::string& tag) { m_tag = tag; return *this; }
			// new
			Builder& AddPipelineStage(const ShaderModule& shader);
			// 1. vertex processing
		/**
		 * @brief 
		 * @tparam 
		 * @param vertexbuffer is for model, otherwise for bufferless rendering or UI
		 * @return
		 */
			Builder& SetVertexProcessingState(bool formodel = true, VkPrimitiveTopology primitiveTopo= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
			
			// 2. tessellation state
			Builder& SetTessellationState() { return *this; };

			// 3. no Geometry shader for Primitive Processing stage

			// 4. Rasterize stage																																												// Enable depth bias : shadow map
			Builder& SetRasterizationState(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, VkBool32 depthBias = VK_FALSE);
			
			// 5. Fragment Proccessing
			Builder& SetDepthState(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp);

			// 6. pixel Proccessing
			Builder& SetMultisampleState(VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT, bool sampleShadingEnable = false);
			Builder& AddColorBlendAttachmentState(VkColorComponentFlags colorWriteMask, VkBool32 blendEnable);
			// todo : make colorAttachmentCount more meaningfull
			Builder& SetColorBlendState(uint32_t colorAttachmentCount);
			

			// dynamic state
			Builder& SetViewportState(uint32_t viewportCount, uint32_t scissorCount);
			/**
		 * @brief
		 * @tparam
		 * @param depth bias
		 * @return
		 */
			Builder& SetDynamicState(VkBool32 depthBias);

		private:
			const class Device& m_device;
			const class PipelineLayout& m_layout;
			const class RenderPass& m_renderpass;
			const class PipelineCache& m_pipelineCache;

			// new
			std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages{};
			VkPipelineVertexInputStateCreateInfo m_vertexInputState{};
			VkPipelineInputAssemblyStateCreateInfo m_inputAssembly{};
			VkPipelineRasterizationStateCreateInfo m_rasterizerState{};
			bool m_depthBiasOn;

			VkPipelineDepthStencilStateCreateInfo m_depthState{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
			VkPipelineMultisampleStateCreateInfo m_multisamplingState{};
			std::vector<VkPipelineColorBlendAttachmentState> m_colorAttachBlendStates{};
			VkPipelineColorBlendStateCreateInfo m_colorBlendState{};
			VkPipelineViewportStateCreateInfo m_viewportState{};

			std::vector<VkDynamicState> m_dynamicStateEnables;
			VkPipelineDynamicStateCreateInfo m_dynamicState{};
			std::string m_tag;
		};

	public:
		VULKAN_NON_COPIABLE(GraphicsPipeline);
		GraphicsPipeline(const Device& device, const PipelineLayout& layout, const RenderPass& renderpass, const PipelineCache& pipelineCache,
			const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages,
			const VkPipelineVertexInputStateCreateInfo& vertexInputState,
			const VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
			const VkPipelineRasterizationStateCreateInfo& rasterizer,
			const VkPipelineDepthStencilStateCreateInfo& depthState,
			const VkPipelineMultisampleStateCreateInfo& multisamplingState,
			const VkPipelineColorBlendStateCreateInfo& colorBlendState,
			const VkPipelineViewportStateCreateInfo& viewportState,
			const VkPipelineDynamicStateCreateInfo& dynamicState,
			const std::string& tag
			);

		~GraphicsPipeline() { Clean(); };
		void Clean();

		const PipelineLayout& GetPipelineLayout()const { return m_layout; }

	private:
		VULKAN_HANDLE(VkPipeline, m_pipeline);
		const class Device& m_device;
		const class PipelineLayout& m_layout;
		const class RenderPass& m_renderpass;
		const class PipelineCache& m_pipelineCache;

	};
}
