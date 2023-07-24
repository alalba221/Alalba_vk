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
		void Clean();
		~UIOverlay();
		void NewFrame();
		bool BufferUpdated();
		void Draw(const vk::CommandBuffers& cmdbuffers, const uint32_t currentCmdBuffer);

		bool needUpdateCmd = false;
		bool visiable = true;

	private:
		const class vk::RenderPass& m_renderPass;

		//std::unique_ptr < vk::ShaderModule > m_vertexShader;
		//std::unique_ptr < vk::ShaderModule > m_fragShader;

		//std::unique_ptr<vk::Allocator> m_allocator;

		//std::unique_ptr<vk::Image> m_fontImage;
		//std::unique_ptr<vk::ImageView> m_fontImageView;
		//std::unique_ptr<vk::Sampler> m_fontSampler;

		std::unique_ptr<vk::CommandPool> m_cmdPool;

		std::unique_ptr<vk::DescriptorPool> m_descPool;
		//std::unique_ptr<vk::DescriptorSetLayout> m_descSetLayout;
		//std::unique_ptr<vk::DescriptorSet> m_descriptorSet;
		
		//std::unique_ptr<vk::PipelineLayout> m_pipelineLayout;
		std::unique_ptr<vk::PipelineCache> m_pipelineCache;
		//std::unique_ptr<vk::GraphicsPipeline> m_pipeline;

		//std::unique_ptr<vk::Buffer> m_vertexBuffer;
		uint32_t vertexCount;
		//std::unique_ptr<vk::Buffer> m_indexBuffer;
		uint32_t indexCount;
	};
}
