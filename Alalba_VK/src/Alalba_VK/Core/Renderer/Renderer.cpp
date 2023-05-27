#include "pch.h"
#include "Renderer.h"
#include "Alalba_VK/Core/Application.h"
#include "Alalba_VK/Core/Scene/Entity.h"

namespace Alalba
{
	Renderer::Renderer(Scene& scene)
		:m_scene(scene)
	{
		Application& app = Application::Get();
		const vk::Device& device = app.GetDevice();

		m_allocator.reset(new vk::Allocator(device, app.GetVulkanInstance(), "Renderer Allocator"));
		
		m_pipelineCache = vk::PipelineCache::Builder(device)
			.SetTag("Rendering PipelineCache")
			.Build();

		m_swapChain = vk::SwapChain::Builder(device, app.GetSurface())
			.SetPresentMode(VK_PRESENT_MODE_MAILBOX_KHR)
			.SetImgSharingMode(VK_SHARING_MODE_EXCLUSIVE)
			.Build();

		m_commandPool = vk::CommandPool::Builder(device)
			.SetTag("Renderer CmdPool")
		//	.SetFlags(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			.SetFlags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			.SetQFamily(device.GetGraphicsQ().GetFamily())
			.Build();
		m_commandBuffers = vk::CommandBuffers::Allocator(device, *m_commandPool)
			.SetTag("CmdBuffers4Graphics")
			.OneTimeSubmit(false)
			.SetSize(3) // one for each image in swapchain
			.Allocate();

		m_depthImage = vk::Image::Builder(device, *m_allocator)
			.SetTag("DepthImage")
			.SetImgType(VK_IMAGE_TYPE_2D)
			.SetImageFormat(device.FindSupportedFormat(
				{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
				VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
			))// this should be compatible with framebuffer
			.SetImageTiling(VK_IMAGE_TILING_OPTIMAL)
			.SetUsageFlags(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			.SetImgExtent(VkExtent3D{ m_swapChain->GetExtent().width, m_swapChain->GetExtent().height,1 })
			.Build();

		m_depthImage->TransitionImageLayout(*m_commandPool, device.GetGraphicsQ(),
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		m_depthImageView = vk::ImageView::Builder(device, *m_depthImage)
			.SetTag("depthImageView")
			.SetViewType(VK_IMAGE_VIEW_TYPE_2D)
			.SetSubresourceAspectFlags(VK_IMAGE_ASPECT_DEPTH_BIT)
			.SetFormat(m_depthImage->GetFormat())
			.Build();

		m_renderPass = vk::RenderPass::Builder(device)
			.SetColorFormat(m_swapChain->GetFormat())
			.SetDepthFormat(m_depthImage->GetFormat()) // this should be compatible with framebuffer
			.SetColorATCHLoadOP(VK_ATTACHMENT_LOAD_OP_CLEAR)
			.SetDepthATCHLoadOP(VK_ATTACHMENT_LOAD_OP_CLEAR)
			.Build();

		m_globalDescPool = vk::DescriptorPool::Builder(device)
			.SetTag("Global Descriptor Pool")
			.SetMaxSets(m_swapChain->GetImgCount())
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, m_swapChain->GetImgCount())
			.Build();

		m_globalDescSetLayout = vk::DescriptorSetLayout::Builder(device)
			// 0 : is bingding index of the binding slot in the set
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			// todo: add lights
			.SetTag("Global Descriptor Set Layout")
			.Build();

		for (int i = 0; i < m_swapChain->GetImgCount(); i++)
		{
			std::string str_int = std::to_string(i);
			std::string tag = std::string{ "Frame buffer for Swapchain" } + str_int;
			m_frameBuffers[i] = vk::FrameBuffer::Builder(device, *m_renderPass)
				.SetTag(tag)
				.SetWidthHeight(m_swapChain->GetExtent().width, m_swapChain->GetExtent().height)
				.AddAttachment(m_swapChain->GetImageView(i))
				.AddAttachment(*m_depthImageView)
				.Build();

			m_inFlightFences[i] =
				vk::Fence::Builder(device)
				.SetTag("InFlightFence " + std::to_string(i))
				.Signaled(true)
				.Build();

			m_imageAvailableSemaphores[i] = vk::Semaphore::Builder(device)
				.SetTag("imageAvailableSemaphore " + std::to_string(i))
				.Build();

			m_renderFinishedSemaphores[i] = vk::Semaphore::Builder(device)
				.SetTag("renderFinishedSemaphore " + std::to_string(i))
				.Build();

			m_globalUniformbuffers[i] = vk::Buffer::Builder(device, *m_allocator)
				.SetTag("Global Uniform Buffer" + std::to_string(i))
				.SetSize(sizeof(GlobalUBO))
				.SetUsage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
				.SetVmaUsage(VMA_MEMORY_USAGE_CPU_TO_GPU)
				.Build();

			m_GlobalDescriptorSets[i] =
				vk::DescriptorSet::Allocator(device, *m_globalDescPool)
				.SetTag("Global Descritor Set " + std::to_string(i))
				.SetDescSetLayout(*m_globalDescSetLayout)
				.Allocate();
			m_GlobalDescriptorSets[i]->
				BindDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, *m_globalUniformbuffers[i], 0, sizeof(GlobalUBO))
				.UpdateDescriptors();
		}

		///  Rendering systems
		//0. basic
		//m_basicDescSetLayout = vk::DescriptorSetLayout::Builder(device)
		//	// 0 : is bingding index of the binding slot in the set
		//	.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
		//	.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
		//	.SetTag("basic descriptor Set Layout")
		//	.Build();
		//std::vector<const vk::DescriptorSetLayout*> basicDescriptorSetLayout =
		//{
		//	m_globalDescSetLayout.get(),
		//	m_basicDescSetLayout.get()
		//};
		//m_basicRenderSys = std::make_unique<BasicRenderSys>(scene, *m_renderPass, basicDescriptorSetLayout, *m_pipelineCache);

		////1. diffracrtion 
		//// no need to handle textures 
		//std::vector<const vk::DescriptorSetLayout*> diffractionDescriptorSetLayout =
		//{
		//	m_globalDescSetLayout.get()
		//};
		//m_diffractionRenderSys = std::make_unique<DiffractionSys>(scene, *m_renderPass, diffractionDescriptorSetLayout, *m_pipelineCache);
		
		/// 2. gltf test
		m_materialDescSetLayout = vk::DescriptorSetLayout::Builder(device)
			// 0 : is bingding index of the binding slot in the set
			.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.SetTag("gltf descriptor Set Layout")
			.Build();
		std::vector<const vk::DescriptorSetLayout*> gltfDescriptorSetLayout =
		{
			m_globalDescSetLayout.get(),
			m_materialDescSetLayout.get()
		};
		m_gltfRenderSys = std::make_unique<glTFRenderSys>(scene, *m_renderPass, gltfDescriptorSetLayout, *m_pipelineCache);
		// other 
	}
	void Renderer::Shutdown()
	{
		const vk::Device& device = Application::Get().GetDevice();
		device.WaitIdle();
	
	/*	m_basicRenderSys->ShutDown();
		m_diffractionRenderSys->ShutDown();*/
		m_gltfRenderSys->ShutDown();

		m_pipelineCache->Clean();
		m_swapChain->Clean();

		m_renderPass->Clean();
		for (auto& framebuffer : m_frameBuffers)
			framebuffer->Clean();

		m_depthImage->Clean();
		m_depthImageView->Clean();

		m_commandBuffers->Clean();
		m_commandPool->Clean();

		for (int i = 0; i < vk::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_inFlightFences[i]->Clean();
			m_imageAvailableSemaphores[i]->Clean();
			m_renderFinishedSemaphores[i]->Clean();
			m_globalUniformbuffers[i]->Clean();
			m_GlobalDescriptorSets[i]->Clean();
		}

		m_globalDescPool->Clean();

		/// rendering systems
		// clean descriptor set layouts
		m_globalDescSetLayout->Clean();
//		m_basicDescSetLayout->Clean();
		m_materialDescSetLayout->Clean();

		m_allocator->Clean();
	}
	void Renderer::Resize()
	{
		Application& app = Application::Get();
		const vk::Device& device = Application::Get().GetDevice();
		device.WaitIdle();
		// clean old framebuffers , depth image , depthimage view and swapchain
		for (int i = 0; i < vk::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_frameBuffers[i]->Clean();
		}
		m_depthImageView->Clean();
		m_depthImage->Clean();
		m_swapChain->Clean();

		// recreate framebuffers , depth image , depthimage view and swapchain
		m_swapChain.reset(new vk::SwapChain(device, app.GetSurface(), VK_PRESENT_MODE_MAILBOX_KHR, VK_SHARING_MODE_EXCLUSIVE));

		m_depthImage.reset(new vk::Image(device, *m_allocator, VK_IMAGE_TYPE_2D,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VkExtent3D{ m_swapChain->GetExtent().width, m_swapChain->GetExtent().height,1 },
			device.FindSupportedFormat(
				{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
				VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
			),
			VK_IMAGE_TILING_OPTIMAL,
			VK_SHARING_MODE_EXCLUSIVE,
			"Recreated depth image"
		));

		m_depthImageView.reset(new vk::ImageView(device, m_depthImage->Handle(),
			VK_IMAGE_ASPECT_DEPTH_BIT, m_depthImage->GetFormat(),
			VK_IMAGE_VIEW_TYPE_2D, "Recreated Depth ImageView"
		));

		for (int i = 0; i < vk::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			std::vector<const vk::ImageView*> attachments;
			attachments.push_back(&(m_swapChain->GetImageView(i)));
			attachments.push_back(m_depthImageView.get());

			m_frameBuffers[i].reset(new vk::FrameBuffer(
				device, *m_renderPass,
				attachments, m_swapChain->GetExtent().width, m_swapChain->GetExtent().height,
				"resized Framebuffer"));
		}

		// Command buffers need to be recreated as they may store
		// references to the recreated frame buffer
		m_commandBuffers->Clean();
		m_commandBuffers = vk::CommandBuffers::Allocator(device, *m_commandPool)
			.SetTag("Resize window CmdBuffers4Graphics")
			.OneTimeSubmit(false)
			.SetSize(3) // one for each image in swapchain
			.Allocate();
		PrepareCommandBuffer(m_scene);

		// SRS - Recreate fences in case number of swapchain images has changed on resize
		for (int i = 0; i < vk::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_inFlightFences[i]->Clean();
			m_inFlightFences[i] =
				vk::Fence::Builder(device)
				.SetTag("Resize InFlightFence " + std::to_string(i))
				.Signaled(true)
				.Build();
		}
		// After recreate SwapChain, the first available image index is 0 (get by vkAcquireNextImageKHR)
		// to keep index coincident, MUST rest m_currentFrame
		m_currentFrame = 0;

	}
	void Renderer::PrepareCommandBuffer(Scene& scene)
	{
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, };// depth from 0 to 1 in vulkan

	
		vk::CommandBuffers& cmdBuffers = (*m_commandBuffers);

		for (int i = 0; i < vk::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = m_renderPass->Handle();
			renderPassInfo.framebuffer = m_frameBuffers[i]->Handle();
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = m_swapChain->GetExtent();
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(m_swapChain->GetExtent().width);
			viewport.height = static_cast<float>(m_swapChain->GetExtent().height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = m_swapChain->GetExtent();
			
			cmdBuffers.BeginRecording(i);
			{
				vkCmdSetViewport(cmdBuffers[i], 0, 1, &viewport);
				vkCmdSetScissor(cmdBuffers[i], 0, 1, &scissor);
				
				/// rendering sys
				vkCmdBeginRenderPass(cmdBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
				
				if(m_BasicSysOn)
					m_basicRenderSys->Render(scene, *m_commandBuffers, *m_GlobalDescriptorSets[i], i);
				if(m_DiffractionSysOn)
					m_diffractionRenderSys->Render(scene, *m_commandBuffers, *m_GlobalDescriptorSets[i], i);
				if (m_gltfSysOn)
					m_gltfRenderSys->Render(*m_commandBuffers, *m_GlobalDescriptorSets[i], i);
		
				
				vkCmdEndRenderPass(cmdBuffers[i]);
			
			}
			cmdBuffers.EndRecording(i);
		}
		
	}

	void Renderer::Update(Scene& scene)
	{
		// update ubo
		auto view = scene.GetAllEntitiesWith<CamComponent>();
		for (auto e : view)
		{
			Entity entity = { e, &scene};
			auto& camera = entity.GetComponent<CamComponent>().m_Camera;

			//ubo.model = glm::mat4(1.0f);
			ubo.view = camera.GetViewMatrix();
			ubo.proj = camera.GetProjectionMatrix();
			ubo.position = camera.GetPosition();
		}
		void* data = m_globalUniformbuffers[m_currentFrame]->MapMemory();
		memcpy(data, &ubo, sizeof(ubo));
		m_globalUniformbuffers[m_currentFrame]->UnMapMemory();
		//
		//m_basicRenderSys->Update(scene);
	}

	void Renderer::DrawFrame(Scene& scene)
	{
		Application& app = Application::Get();
		const vk::Device& device = app.GetDevice();

		m_inFlightFences[m_currentFrame]->Wait(UINT64_MAX);

		VkResult result = vkAcquireNextImageKHR(device.Handle(), m_swapChain->Handle(), UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame]->Handle(), VK_NULL_HANDLE, &m_currentFrame);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			Resize();
			return;
		}
		else
		{
			ALALBA_ASSERT(result == VK_SUCCESS, "Acquire Next Image Failed");
		}
		// update ubo
		Update(scene);
		
		m_inFlightFences[m_currentFrame]->Reset();

		// TODO: abstract to a new submit method
		VkCommandBuffer commandBuffers[]{ (*m_commandBuffers)[m_currentFrame] };
		VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame]->Handle() };
		VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame]->Handle() };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = commandBuffers;
		VkResult err;
		err = vkQueueSubmit(device.GetGraphicsQ().Handle(),
			1, &submitInfo, m_inFlightFences[m_currentFrame]->Handle());
		ALALBA_ASSERT(err == VK_SUCCESS, "Q submit failed");


		// TODO: abstract to a new present method
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_swapChain->Handle() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_currentFrame;
		presentInfo.pResults = nullptr; // Optional

		result = vkQueuePresentKHR(device.GetGraphicsQ().Handle(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			Resize();
			return;
		}
		else
		{
			ALALBA_ASSERT(result == VK_SUCCESS, "Queue Present Failed");
		}
		m_currentFrame = (m_currentFrame + 1) % m_swapChain->GetImgCount();

	}

}
