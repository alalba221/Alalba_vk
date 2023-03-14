#pragma once
#include "Alalba_VK/Vulkan/SwapChain.h"
#include "Alalba_VK/Vulkan/ShaderModule.h"
#include "Alalba_VK/Vulkan/Image.h"
#include "Alalba_VK/Vulkan/ImageView.h"
#include "Alalba_VK/Vulkan/GraphicsPipeline.h"
#include "Alalba_VK/Vulkan/FrameBuffer.h"
#include "Alalba_VK/Vulkan/RenderPass.h"
#include "Alalba_VK/Vulkan/PipelineLayout.h"
#include "Alalba_VK/Vulkan/CommandPool.h"
#include "Alalba_VK/Vulkan/CommandBuffers.h"
#include "Alalba_VK/Vulkan/Fence.h"
#include "Alalba_VK/Vulkan/Semaphore.h"
#include "Alalba_VK/Vulkan/Buffer.h"
#include "Alalba_VK/Vulkan/PipelineCache.h"


// test
#include <glm/glm.hpp>

#include "Alalba_VK/Vulkan/DescriptorSetLayout.h"
#include "Alalba_VK/Vulkan/DescriptorPool.h"
#include "Alalba_VK/Vulkan/DescriptorSet.h"

// 
#include "Alalba_VK/Assets/Texture.h"

namespace Alalba
{	
	class Model;
	class Camera;
};
namespace vk
{
	class Device;
	class Allocator;
	class VulkanRenderer
	{

	public:
		VulkanRenderer(const VulkanRenderer&) = delete;
		
		//inline static VulkanRenderer& GetInstance(const Device& device, Allocator& allocator) 
		//{ 
		//	static VulkanRenderer s_vulkanRenderer(device, allocator); 
		//	return s_vulkanRenderer; 
		//};

		void Shutdown();
		void RecreateSwapChainAndFramebuffers();
		VulkanRenderer(const Device& device) :m_device(device){};
		const FrameBuffer& GetFramebuffer(uint32_t index) const { return *(m_framebuffers[index].get()); };
		// cmdBufferIndex =  imageIndex
		void EncodeCommand(const uint32_t cmdBufferIndex, const uint32_t imageIndex, const Alalba::Model& mesh,
			const Alalba::Texture& texture);

		void DrawFrame(const Alalba::Model& mesh, const Alalba::Texture& texture, const Alalba::Camera& camera);

		void Init(const std::string& vertshader, const std::string& fragshader, const bool quad = false);

		const CommandPool& GetCommandPool() const { return *m_cmdPool4Graphics.get(); }

	private:
		
		const class Device& m_device;
		
		std::unique_ptr<Allocator> m_allocator;
		
		std::unique_ptr<SwapChain> m_SwapChain;
		std::unique_ptr<GraphicsPipeline> m_graphicsPipeline;

		std::unique_ptr<RenderPass> m_renderPass;
		std::unique_ptr<ShaderModule> m_vertShaderModule;
		std::unique_ptr<ShaderModule> m_fragShaderModule;
		std::unique_ptr<PipelineLayout> m_pipelineLayout;

		std::unique_ptr<Image> m_depthImage;
		std::unique_ptr<ImageView> m_depthImageView;
		
		std::vector<std::unique_ptr<FrameBuffer>> m_framebuffers;

		std::unique_ptr<CommandPool> m_cmdPool4Graphics;
		std::unique_ptr<CommandBuffers> m_cmdBuffers;

		std::vector<std::unique_ptr<Fence>> m_inFlightFences;
		std::vector<std::unique_ptr<Semaphore>> m_imageAvailableSemaphores;
		std::vector<std::unique_ptr<Semaphore>> m_renderFinishedSemaphores;

		std::unique_ptr<PipelineCache> m_pipelineCache;

		// uniform buffer
		//test 
		std::unique_ptr<DescriptorPool> m_globalDescPool;

		std::unique_ptr<DescriptorSetLayout >m_globalDescSetLayout;
		std::vector<std::unique_ptr<DescriptorSet>> m_globalDescSets;

		std::unique_ptr<DescriptorSetLayout >test_textureSetLayout;
		std::vector<std::unique_ptr<DescriptorSet>> test_textureDescSets;

		
		struct UniformBufferObject {
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
			glm::vec3 position;
		};

		std::vector< std::unique_ptr<Buffer> > m_globalUniformbuffers;
	
		uint32_t m_currentFrame = 0;

		bool m_quad;
	};
}


