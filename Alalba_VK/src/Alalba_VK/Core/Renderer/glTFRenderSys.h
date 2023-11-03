#pragma once

#include "Alalba_VK/Vulkan/PipelineLayout.h"
#include "Alalba_VK/Vulkan/GraphicsPipeline.h"
#include "Alalba_VK/Vulkan/ShaderModule.h"
#include "Alalba_VK/Core/Scene/Scene.h"
#include "Alalba_VK/Vulkan/DescriptorPool.h"
#include "Alalba_VK/Vulkan/DescriptorSet.h"
#include "Alalba_VK/Vulkan/Buffer.h"
#include "Alalba_VK/Vulkan/SwapChain.h"
#include "Alalba_VK/Vulkan/FrameBuffer.h"
#include "Alalba_VK/Core/ImGui/UIOverlay.h"

namespace vk
{
	class RenderPass;
	class DescriptorSetLayout;
	class PipelineCache;
	class CommandBuffers;
}
namespace Alalba
{

	class glTFRenderSys
	{
	public:
		glTFRenderSys(
			Scene& scene,
			const vk::RenderPass& renderpass,
			const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts,
			const vk::PipelineCache& pipelineCache);
		~glTFRenderSys() {  };


		// rendering related command, mainly record command into command buffers
		void BuildCommandBuffer(const vk::RenderPass& renderpass, const vk::FrameBuffer& framebuffer, VkExtent2D areaExtend,
			const vk::DescriptorSet& globalDescSet,
			const vk::CommandBuffers& cmdBuffers, const uint32_t currentCmdBuffer);

		void Update();
		void ShutDown();



	private:

		std::unique_ptr < vk::ShaderModule > m_vertexShader;
		std::unique_ptr < vk::ShaderModule > m_fragShader;
		std::unique_ptr<vk::GraphicsPipeline> m_graphicsPipeline;
		std::unique_ptr<vk::PipelineLayout> m_pipelineLayout;
		
		//GLTFSys& m_loader;
		Scene& m_scene;
		

	private:
		void CreateShaders(const std::string& vertex, const std::string& frag);
		void CreatePipelineLayout(const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts);
		void CreatePipeline(const vk::RenderPass& renderpass, const vk::PipelineCache& pipelineCache);
		
	public:

	};
}



