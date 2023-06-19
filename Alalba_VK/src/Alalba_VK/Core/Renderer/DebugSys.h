#pragma once

#include "Alalba_VK/Vulkan/DescriptorSetLayout.h"
#include "Alalba_VK/Vulkan/GraphicsPipeline.h"
#include "Alalba_VK/Vulkan/ShaderModule.h"
#include "Alalba_VK/Vulkan/Allocator.h"
#include "Alalba_VK/Vulkan/DescriptorPool.h"
#include "Alalba_VK/Vulkan/DescriptorSet.h"
#include "Alalba_VK/Vulkan/PipelineLayout.h"
#include "Alalba_VK/Vulkan/PipelineCache.h"

namespace vk
{
	class RenderPass;
	class FrameBuffer;
	class CommandBuffers;
}
namespace Alalba
{

	class DebugSys
	{
	public:
		/** @brief
		 *
		 *@ param imageLayout is the layout that the image subresources accessible from imageView will be in at the time this descriptor is accessed

		 */
		DebugSys(const vk::RenderPass& renderpass, 
			VkImageLayout imageLayout, const vk::ImageView& imageview, const vk::Sampler& sampler);
		~DebugSys() {  };


		// rendering related command, mainly record command into command buffers
	// 	void Render(vk::CommandBuffers& cmdBuffers, const int currentCmdBuffer);
		void BuildCommandBuffer(const vk::RenderPass& renderpass, const vk::FrameBuffer& framebuffer, VkExtent2D areaExtend, 
			const vk::CommandBuffers& cmdBuffers, const uint32_t currentCmdBuffer);

		void Update();
		void ShutDown();

	private:

		struct DebugUbo
		{
			//glm::mat4 lightView;
			//glm::mat4 lightProject;
			// Used for depth map visualization
			float zNear = 0.1f ;
			float zFar = 100.0f;
		};


		std::unique_ptr<vk::ShaderModule> m_vertexShader;
		std::unique_ptr<vk::ShaderModule> m_fragShader;
		std::unique_ptr<vk::PipelineCache> m_pipelineCache;
		std::unique_ptr<vk::GraphicsPipeline> m_graphicsPipeline;
		std::unique_ptr<vk::DescriptorSetLayout> m_descSetLayout;
		std::unique_ptr<vk::PipelineLayout> m_pipelineLayout;
		std::unique_ptr<vk::DescriptorPool> m_descPool;
		std::unique_ptr<vk::DescriptorSet> m_descSet;
		std::unique_ptr<vk::Allocator> m_allocator;
		std::unique_ptr<vk::Buffer> m_debugUBO;

		DebugUbo m_ubo{};



	private:
		void CreateShaders(const std::string& vertex, const std::string& frag);
		void PrepareUniformBuffer();
		void SetupDescriptorSetLayout();
		void PreparePipeline(const vk::RenderPass& renderpass);
		void PrepareDescripterSet(VkImageLayout imageLayout, const vk::ImageView& imageview, const vk::Sampler& sampler);
		
	};
}


