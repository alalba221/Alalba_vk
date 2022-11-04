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
		VulkanRenderer(const Device& device, Allocator& allocator) :m_device(device), m_allocator(allocator) {};
		const FrameBuffer& GetFramebuffer(uint32_t index) const { return *(m_framebuffers[index].get()); };
		void EncodeCommand(const uint32_t cmdBufferIndex, const uint32_t imageIndex, const Alalba::Mesh& mesh);
		void DrawFrame(const Alalba::Mesh& mesh);
		void Init();

		const CommandPool& GetCommandPool() const { return *m_cmdPool4Graphics.get(); }

	private:
		
		const class Device& m_device;
		class Allocator& m_allocator;
		
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
		std::vector<std::unique_ptr<Buffer>> m_uniforbuffers;
		

		uint32_t m_currentFrame = 0;

	};
}


