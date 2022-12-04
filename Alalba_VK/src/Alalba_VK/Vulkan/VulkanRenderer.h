#pragma once
#include "SwapChain.h"
#include "ShaderModule.h"
#include "Image.h"
#include "ImageView.h"
#include "GraphicsPipeline.h"
#include "FrameBuffer.h"
#include "RenderPass.h"
#include "PipelineLayout.h"
#include "CommandPool.h"
#include "CommandBuffers.h"
#include "Fence.h"
#include "Semaphore.h"
#include "Buffer.h"

// test
#include "glm/glm.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#include "DescriptorSetLayout.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"

// 
#include "Alalba_VK/Assets/Texture.h"

namespace Alalba
{	
	class Mesh;
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
		void EncodeCommand(const uint32_t cmdBufferIndex, const uint32_t imageIndex, const Alalba::Mesh& mesh);
		void DrawFrame(const Alalba::Mesh& mesh);

		void Init(const Alalba::Texture& texture);

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

		// uniform buffer
		//test 
		std::unique_ptr<DescriptorSetLayout >m_globalDescSetLayout;
		std::unique_ptr<DescriptorPool> m_globalDescPool;
		std::vector<std::unique_ptr<DescriptorSet>> m_globalDescSets;
		
		struct UniformBufferObject {
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
		};
		
		std::vector< std::unique_ptr<Buffer> > m_globalUniformbuffers;
	
		void updateUniformBuffer(uint32_t currentImage) {
			static auto startTime = std::chrono::high_resolution_clock::now();
			UniformBufferObject ubo{};
			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
			ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			ubo.proj = glm::perspective(glm::radians(45.0f), ((float)m_SwapChain->GetExtent().width) / ((float)m_SwapChain->GetExtent().width), 0.1f, 10.0f);
			ubo.proj[1][1] *= -1;

			void* data = m_globalUniformbuffers[currentImage]->MapMemory();
			memcpy(data, &ubo, sizeof(ubo));
			m_globalUniformbuffers[currentImage]->UnMapMemory();
		}

		uint32_t m_currentFrame = 0;
	};
}


