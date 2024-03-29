#pragma once

#include "Alalba_VK/Vulkan/PipelineLayout.h"
#include "Alalba_VK/Vulkan/GraphicsPipeline.h"
#include "Alalba_VK/Vulkan/ShaderModule.h"
#include "Alalba_VK/Core/Scene/Scene.h"
#include "Alalba_VK/Vulkan/DescriptorPool.h"
#include "Alalba_VK/Vulkan/DescriptorSet.h"
#include "Alalba_VK/Vulkan/Buffer.h"
#include "Alalba_VK/Vulkan/SwapChain.h"

#include "Alalba_VK/Vulkan/Allocator.h"
namespace vk
{
	class RenderPass;
	class DescriptorSetLayout;
	class PipelineCache;
	class CommandBuffers;
}
namespace Alalba
{	

	class BasicRenderSys
	{
	public:
		BasicRenderSys(
			Scene& scene,
			const vk::RenderPass& renderpass, 
			const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts, 
			const vk::PipelineCache& pipelineCache);
		~BasicRenderSys() {  };

	
		// rendering related command, mainly record command into command buffers
		void Render(Scene& scene, vk::CommandBuffers& cmdBuffers, 
			const vk::DescriptorSet& globalDescSet,
			const int currentCmdBuffer);

		void Update(Scene& scene);
		void ShutDown();

	private:

		struct ModelUBO 
		{
			glm::mat4 model; 
		};

		std::unique_ptr < vk::ShaderModule > m_vertexShader;
		std::unique_ptr < vk::ShaderModule > m_fragShader;
		std::unique_ptr<vk::GraphicsPipeline> m_graphicsPipeline;
		std::unique_ptr<vk::PipelineLayout> m_pipelineLayout;
		std::unique_ptr<vk::DescriptorPool> m_descPool;

		// need a place to store ubos and descriptorsets till the end of the app.
		std::unordered_map<std::string, std::shared_ptr<vk::DescriptorSet> > m_modelDescSets;
		std::unique_ptr<vk::Allocator> m_allocator;

		std::unordered_map<std::string, std::shared_ptr<vk::Buffer> > m_modelUBOs;

	private:
		void CreateShaders(const std::string& vertex, const std::string& frag);
		void CreatePipelineLayout(const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts);
		void CreatePipeline(const vk::RenderPass& renderpass, const vk::PipelineCache& pipelineCache);
		void PrepareDescriptorSets(Scene& scene, const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts);

	};
}


