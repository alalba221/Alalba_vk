#pragma once
#include "Alalba_VK/Vulkan/ShaderModule.h"
#include "Alalba_VK/Vulkan/Image.h"
#include "Alalba_VK/Vulkan/ImageView.h"
#include "Alalba_VK/Vulkan/Sampler.h"

#include "Alalba_VK/Vulkan/FrameBuffer.h"
#include "Alalba_VK/Vulkan/RenderPass.h"
#include "Alalba_VK/Vulkan/CommandPool.h"
#include "Alalba_VK/Vulkan/CommandBuffers.h"

#include "Alalba_VK/Vulkan/Buffer.h"

#include "Alalba_VK/Vulkan/DescriptorPool.h"
#include "Alalba_VK/Vulkan/DescriptorSet.h"
#include "Alalba_VK/Vulkan/DescriptorSetLayout.h"

#include "Alalba_VK/Vulkan/PipelineLayout.h"
#include "Alalba_VK/Vulkan/PipelineCache.h"
#include "Alalba_VK/Vulkan/GraphicsPipeline.h"

#include "Alalba_VK/Core/Scene/Scene.h"
#include "Alalba_VK/Vulkan/Allocator.h"

#include "Alalba_VK/Vulkan/SwapChain.h"
namespace Alalba 
{
	//class DebugSys;
	class ShadowMappingSys
	{
	public:
		ShadowMappingSys( Scene& scene, const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts);
		~ShadowMappingSys() {};

		void GenerateShadowMapp(vk::CommandBuffers& cmdBuffers, const uint32_t currentCmdBuffer);
	
		//// rendering related command, mainly record command into command buffers
		//void Render(Scene& scene, vk::CommandBuffers& cmdBuffers,
		//	const vk::DescriptorSet& globalDescSet,
		//	const int currentCmdBuffer);

		void Update(Scene& scene, uint32_t currentFrame);
		void ShutDown();

		const vk::ImageView& GetImageView(uint32_t i) const { return *m_depthImageViews[i]; }
		const vk::Sampler& GetSampler(uint32_t i)const { return *m_samplers[i]; }
		const vk::Image& GetImage(uint32_t i) const { return *m_depthImages[i]; }

		static constexpr int SHADOWMAP_DIM = 2048;

	private:
		float zNear = 0.1f;
		float zFar = 100.0f;
		struct ShadowMappUbo
		{
			glm::mat4 lightView;
			glm::mat4 lightProject;
			
			// Used for depth map visualization
			float zNear ;
			float zFar ;
		};

		std::unique_ptr<vk::Allocator> m_allocator;
		std::unique_ptr<vk::CommandPool> m_cmdPool;

		std::vector<std::unique_ptr<vk::Image>> m_depthImages{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
		std::vector<std::unique_ptr<vk::ImageView>> m_depthImageViews{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
		std::vector<std::unique_ptr<vk::Sampler>> m_samplers{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };

		std::unique_ptr<vk::RenderPass> m_renderPass;
		std::vector< std::unique_ptr<vk::FrameBuffer> > m_framebuffers{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };

		std::vector<std::unique_ptr<vk::Buffer>> m_shadowMapUBOs{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };
		std::unique_ptr<vk::DescriptorSetLayout> m_descSetLayout;


		std::unique_ptr<vk::ShaderModule> m_vertShader;
		//std::unique_ptr<vk::ShaderModule> m_offScreenFragShader;
		
		std::unique_ptr<vk::PipelineLayout> m_pipelineLayout;
		std::unique_ptr<vk::PipelineCache> m_pipelineCache;
		std::unique_ptr<vk::GraphicsPipeline> m_pipeline;
		
		std::unique_ptr<vk::DescriptorPool> m_descPool;
		std::vector<std::unique_ptr<vk::DescriptorSet>> m_descriptorSets{ vk::SwapChain::MAX_FRAMES_IN_FLIGHT };

		
		Scene& m_scene;
	private:
		void PrepareOffScreenRenderPass();
		void PrepareOffscreenFramebuffer();
		void PrepareUniformBuffer();
		void SetupDescriptorSetLayout(const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts);

		void CreateShaders(const std::string& vertexShader, const std::string& fragShader);
		void PreparePipeline();
		void PrepareOffScreenDescripterSets();

		float m_depthBiasConstant = 1.25f;
		// Slope depth bias factor, applied depending on polygon's slope
		float m_depthBiasSlope = 1.75f;
		ShadowMappUbo m_ubo{};

		//friend class DebugSys;
	};
}
