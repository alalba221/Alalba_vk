#pragma once
#include "Alalba_VK/Vulkan/PipelineLayout.h"
#include "Alalba_VK/Vulkan/GraphicsPipeline.h"
#include "Alalba_VK/Vulkan/ShaderModule.h"
#include "Alalba_VK/Core/Scene/Scene.h"
#include "Alalba_VK/Vulkan/DescriptorPool.h"
#include "Alalba_VK/Vulkan/DescriptorSet.h"
#include "Alalba_VK/Vulkan/Buffer.h"
#include "Alalba_VK/Vulkan/SwapChain.h"


namespace vk
{
	class RenderPass;
	class DescriptorSetLayout;
	class PipelineCache;
	class CommandBuffers;
}

namespace Alalba 
{
	class ShadowMappingSys
	{
	//public:
	//	ShadowMappingSys(
	//		Scene& scene,
	//		const vk::RenderPass& renderpass,
	//		const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts,
	//		const vk::PipelineCache& pipelineCache);
	//	~ShadowMappingSys() {  };


	//	// rendering related command, mainly record command into command buffers
	//	void Render(Scene& scene, vk::CommandBuffers& cmdBuffers,
	//		const vk::DescriptorSet& globalDescSet,
	//		const int currentCmdBuffer);

	//	void Update(Scene& scene);
	//	void ShutDown();

	//private:

	//	struct LightUBO
	//	{
	//		glm::mat4 model;
	//	};

	//	std::unique_ptr < vk::ShaderModule > m_vertexShader;
	//	std::unique_ptr < vk::ShaderModule > m_fragShader;
	//	std::unique_ptr<vk::GraphicsPipeline> m_pipeline;
	//	std::unique_ptr<vk::PipelineLayout> m_pipelineLayout;
	//	std::unique_ptr<vk::DescriptorPool> m_descPool;

	//	std::unique_ptr<vk::RenderPass> m_renderPass;
	//	std::unique_ptr<vk::FrameBuffer> m_framebuffer;
	//	std::unique_ptr<vk::Image> m_depthImage;
	//	std::unique_ptr<vk::ImageView> m_depthImageView;

	//	std::unique_ptr<vk::Allocator> m_allocator;

	//	std::unordered_map<std::string, std::shared_ptr<vk::Buffer> > m_modelUBOs;

	//private:
	//	void CreateShaders(const std::string& vertex, const std::string& frag);
	//	void CreatePipelineLayout(const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts);
	//	void CreatePipeline(const vk::RenderPass& renderpass, const vk::PipelineCache& pipelineCache);
	//	void PrepareDescriptorSets(Scene& scene, const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts);
	};
}
