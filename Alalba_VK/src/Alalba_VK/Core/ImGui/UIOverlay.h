#pragma once
#include "Alalba_VK/Vulkan/ShaderModule.h"
#include "Alalba_VK/Vulkan/Image.h"
#include "Alalba_VK/Vulkan/ImageView.h"
#include "Alalba_VK/Vulkan/Sampler.h"

#include "Alalba_VK/Vulkan/Buffer.h"
#include "Alalba_VK/Vulkan/Allocator.h"

#include "Alalba_VK/Vulkan/PipelineLayout.h"
#include "Alalba_VK/Vulkan/PipelineCache.h"
#include "Alalba_VK/Vulkan/GraphicsPipeline.h"

#include "Alalba_VK/Vulkan/DescriptorPool.h"
#include "Alalba_VK/Vulkan/DescriptorSet.h"
#include "Alalba_VK/Vulkan/DescriptorSetLayout.h"

#include "Alalba_VK/Vulkan/Allocator.h"
#include "glm/glm.hpp"
#include "imgui.h"
namespace vk
{
	//class Device;
	class RenderPass;
	class CommandBuffers;

}
namespace Alalba
{
	class UIOverlay
	{
	public:
		UIOverlay(const vk::RenderPass& renderPass);
		
		void NewFrame();
		bool UpdateBuffers();
		void Draw(const vk::CommandBuffers& cmdbuffers, const uint32_t currentCmdBuffer);

	public:
		bool visible = true;
		bool updated = false;
		float scale = 1.0f;

		struct PushConstBlock {
			glm::vec2 scale;
			glm::vec2 translate;
		} pushConstBlock;

		static std::array<VkVertexInputBindingDescription, 1> binding_descriptions;
		static std::array<VkVertexInputBindingDescription, 1>& GetBindingDescriptions()
		{
			// position
			binding_descriptions[0].binding = 0;
			binding_descriptions[0].stride = sizeof(ImDrawVert);
			binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return binding_descriptions;
		}
		static std::array<VkVertexInputAttributeDescription, 3> attribute_descriptions;
		static std::array<VkVertexInputAttributeDescription, 3>& GetAttributeDescriptions()
		{

			attribute_descriptions[0].binding = 0;
			attribute_descriptions[0].location = 0;
			attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attribute_descriptions[0].offset = offsetof(ImDrawVert, pos);

			attribute_descriptions[1].binding = 0;
			attribute_descriptions[1].location = 1;
			attribute_descriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
			attribute_descriptions[1].offset = offsetof(ImDrawVert, uv);

			attribute_descriptions[2].binding = 0;
			attribute_descriptions[2].location = 2;
			attribute_descriptions[2].format = VK_FORMAT_R8G8B8A8_UNORM;
			attribute_descriptions[2].offset = offsetof(ImDrawVert, col);

			return attribute_descriptions;
		}

	private:
		void PrepareResources();
		void PreparePipeline();

	private:
		const class vk::RenderPass& m_renderPass;

		std::unique_ptr < vk::ShaderModule > m_vertexShader;
		std::unique_ptr < vk::ShaderModule > m_fragShader;

		std::unique_ptr<vk::Allocator> m_allocator;

		std::unique_ptr<vk::Image> m_fontImage;
		std::unique_ptr<vk::ImageView> m_fontImageView;
		std::unique_ptr<vk::Sampler> m_fontSampler;

		std::unique_ptr<vk::CommandPool> m_cmdPool;

		std::unique_ptr<vk::DescriptorPool> m_descPool;
		std::unique_ptr<vk::DescriptorSetLayout> m_descSetLayout;
		std::unique_ptr<vk::DescriptorSet> m_descriptorSet;
		
		std::unique_ptr<vk::PipelineLayout> m_pipelineLayout;
		std::unique_ptr<vk::PipelineCache> m_pipelineCache;
		std::unique_ptr<vk::GraphicsPipeline> m_pipeline;

		std::unique_ptr<vk::Buffer> m_vertexBuffer;
		uint32_t vertexCount;
		std::unique_ptr<vk::Buffer> m_indexBuffer;
		uint32_t indexCount;

	};
}
