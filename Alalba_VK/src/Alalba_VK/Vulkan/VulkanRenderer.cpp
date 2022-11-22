#include "pch.h"
#include "VulkanRenderer.h"
#include "Device.h"
#include "Allocator.h"
#include "Alalba_VK/Core/Application.h"

#include "Alalba_VK/Assets/Vertex.h"
#include "Alalba_VK/Assets/Model.h"
namespace vk
{

	void VulkanRenderer::Init()
	{
		m_SwapChain = SwapChain::Builder(m_device, Alalba::Application::Get().GetSurface())
			.SetPresentMode(VK_PRESENT_MODE_MAILBOX_KHR)
			.SetImgSharingMode(VK_SHARING_MODE_EXCLUSIVE)
			.Build();

		m_vertShaderModule = ShaderModule::Builder(m_device)
			// path relative to Sandbox
			.SetTag("vertexShader")
			.SelectSpvFile("../Alalba_VK/src/Alalba_VK/Shaders/vert.spv")
			.SetShaderStageBits(VK_SHADER_STAGE_VERTEX_BIT)
			.Build();

		m_fragShaderModule = ShaderModule::Builder(m_device)
			// path relative to Sandbox
			.SetTag("fragShader")
			.SelectSpvFile("../Alalba_VK/src/Alalba_VK/Shaders/frag.spv")
			.SetShaderStageBits(VK_SHADER_STAGE_FRAGMENT_BIT)
			.Build();

		m_renderPass = RenderPass::Builder(m_device)
			.SetColorFormat(m_SwapChain->GetFormat())
			.SetDepthFormat(m_device.FindSupportedFormat(
				{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
				VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
			)) // this should be compatible with framebuffer
			.SetColorATCHLoadOP(VK_ATTACHMENT_LOAD_OP_CLEAR)
			.SetDepthATCHLoadOP(VK_ATTACHMENT_LOAD_OP_CLEAR)
			.Build();
		
		m_depthImage = Image::Builder(m_device, m_allocator)
			.SetTag("DepthImage")
			.SetImgType(VK_IMAGE_TYPE_2D)
			.SetImageFormat(m_device.FindSupportedFormat(
				{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
				VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
			))// this should be compatible with framebuffer
			.SetImageTiling(VK_IMAGE_TILING_OPTIMAL)
			.SetUsageFlags(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			.SetImgExtent(VkExtent3D{ m_SwapChain->GetExtent().width, m_SwapChain->GetExtent().height,1 })
			.Build();

		m_depthImageView = ImageView::Builder(m_device, *m_depthImage.get())
			.SetTag("depthImageView")
			.SetViewType(VK_IMAGE_VIEW_TYPE_2D)
			.SetSubresourceAspectFlags(VK_IMAGE_ASPECT_DEPTH_BIT)
			.SetFormat(m_depthImage->GetFormat())
			.Build();

		/// test 
		// descriptor set layout
		m_globalDescSetLayout = DescriptorSetLayout::Builder(m_device)
			// 0 : is bingding index to set layout
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.SetTag("Global Descriptor Set Layout")
			.Build();

		m_pipelineLayout = PipelineLayout::Builder(m_device)
			.SetTag("Pipeline layout")
			.BindDescriptorSetLayout(*m_globalDescSetLayout.get())
			.Build();

		m_graphicsPipeline = GraphicsPipeline::Builder(m_device, *m_pipelineLayout.get(), *m_renderPass.get(),
			*m_vertShaderModule.get(), *m_fragShaderModule.get())
			.SetAssemblyTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
			.SetPolygonMode(VK_POLYGON_MODE_FILL)
			.SetScirrorExtent(m_SwapChain->GetExtent())
			.SetViewPortWidth(static_cast<float>(m_SwapChain->GetExtent().width))
			.SetViewPortHeight(static_cast<float>(m_SwapChain->GetExtent().height))
			.Build();

		for (int i = 0; i < m_SwapChain->GetImgCount(); i++)
		{
			std::string str_int = std::to_string(i);
			std::string tag = std::string{ "Frame buffer for Swapchain" } + str_int;
			m_framebuffers.push_back(FrameBuffer::Builder(m_device, *m_renderPass.get())
				.SetTag(tag)
				.SetWidthHeight(m_SwapChain->GetExtent().width, m_SwapChain->GetExtent().height)
				.AddAttachment(m_SwapChain->GetImageView(i))
				//.AddAttachment(*m_depthImageView.get())
				.Build());
		}

		m_cmdPool4Graphics = CommandPool::Builder(m_device)
			.SetTag("GraphicsCmdPool")
			.SetFlags(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			.SetQFamily(Alalba::Application::Get().GetVulkanInstance().GetPhysicalDevice().GetQFamilies().graphics.value())
			.Build();

		m_cmdBuffers = CommandBuffers::Allocator(m_device, *m_cmdPool4Graphics.get())
			.SetTag("CmdBuffers4Graphics")
			.SetSize(3) // one for each image in swapchain
			.Allocate();

		//
		for (int i = 0; i < m_SwapChain->GetImgCount(); i++)
		{
			m_inFlightFences.push_back(
				Fence::Builder(m_device)
				.SetTag("InFlightFence "+ std::to_string(i))
				.Signaled(true)
				.Build()
				);

			m_imageAvailableSemaphores.push_back(Semaphore::Builder(m_device)
				.SetTag("imageAvailableSemaphore "+ std::to_string(i))
				.Build()
				);

			m_renderFinishedSemaphores.push_back(Semaphore::Builder(m_device)
				.SetTag("renderFinishedSemaphore " + std::to_string(i))
				.Build()
				);
		}
		
		// uniform buffer
		m_globalDescPool = DescriptorPool::Builder(m_device)
			.SetTag("Descriptor Pool")
			.SetMaxSets(m_SwapChain->GetImgCount())
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, m_SwapChain->GetImgCount())
			.Build();
		// test
		
		for (int i = 0; i < m_SwapChain->GetImgCount(); i++)
		{
			m_globalUniformbuffers.push_back(
				Buffer::Builder(m_device, m_allocator)
				.SetTag("Uniform Buffer" + std::to_string(i))
				.SetSize(sizeof(UniformBufferObject))
				.SetUsage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
				.SetVmaUsage(VMA_MEMORY_USAGE_CPU_TO_GPU)
				.Build()
			);
			m_globalDescSets.push_back(
				DescriptorSet::Allocator(m_device, *m_globalDescPool.get())
				.SetTag("Global Set " + std::to_string(i))
				.SetDescSetLayout(*m_globalDescSetLayout.get())
				.Allocate()
			);

			m_globalDescSets[i]->BindDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, *m_globalUniformbuffers[i].get(), 0, sizeof(UniformBufferObject))
				.UpdateDescriptors();
		}
		

		// test
		//for (int i = 0; i < m_SwapChain->GetImgCount(); i++)
		//{
		//	void* data = m_allocator.MapMemory(m_globalUniformbuffers[i]->GetAllocation());
		//	datas.push_back(data);
		//}

	}
	void VulkanRenderer::Shutdown()
	{
		vkDeviceWaitIdle(m_device.Handle());
		m_pipelineLayout->Clean();
		m_graphicsPipeline->Clean();
		m_SwapChain->Clean();

		m_vertShaderModule->Clean();
		m_fragShaderModule->Clean();

		m_renderPass->Clean();
		for(auto& framebuffer:m_framebuffers)
			framebuffer->Clean();

		m_depthImage->Clean();
		m_depthImageView->Clean();

		m_cmdBuffers->Clean();
		m_cmdPool4Graphics->Clean();
		
		for (int i = 0; i < m_inFlightFences.size(); i++)
		{
			m_inFlightFences[i]->Clean();
			m_imageAvailableSemaphores[i]->Clean();
			m_renderFinishedSemaphores[i]->Clean();

			m_globalUniformbuffers[i]->Clean();
		}

		m_globalDescSetLayout->Clean();
		
		m_globalDescPool->Clean();
	}
	void VulkanRenderer::RecreateSwapChainAndFramebuffers()
	{
		vkDeviceWaitIdle(m_device.Handle());
		// clean old framebuffers , depth image , depthimage view and swapchain
		for (int i = 0; i < m_SwapChain->GetImgCount(); i++)
		{
			m_framebuffers[i]->Clean();
		}
		m_depthImageView->Clean();
		m_depthImage->Clean();
		m_SwapChain->Clean();

		// recreate framebuffers , depth image , depthimage view and swapchain
		m_SwapChain.reset(new SwapChain(m_device, Alalba::Application::Get().GetSurface(), VK_PRESENT_MODE_MAILBOX_KHR, VK_SHARING_MODE_EXCLUSIVE));
		
		m_depthImage.reset(new Image(m_device,m_allocator, VK_IMAGE_TYPE_2D,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VkExtent3D{ m_SwapChain->GetExtent().width, m_SwapChain->GetExtent().height,1 },
			m_device.FindSupportedFormat(
				{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
				VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
			),
			VK_IMAGE_TILING_OPTIMAL,
			"Recreated depth image"
			));

		m_depthImageView.reset(new ImageView(m_device,m_depthImage->Handle(),
			VK_IMAGE_ASPECT_DEPTH_BIT, m_depthImage->GetFormat(),
			VK_IMAGE_VIEW_TYPE_2D, "Recreated Depth ImageView"
		));

		for (int i = 0; i < m_SwapChain->GetImgCount(); i++)
		{
			std::vector<const ImageView*> attachments;
			attachments.push_back(&(m_SwapChain->GetImageView(i)));
		
			m_framebuffers[i].reset(new FrameBuffer(
				m_device,*m_renderPass.get(),
				attachments, m_SwapChain->GetExtent().width, m_SwapChain->GetExtent().height,
				"resized Framebuffer"));
		}
	}
	void VulkanRenderer::EncodeCommand(const uint32_t cmdBufferIndex,const uint32_t imageIndex, const Alalba::Mesh& mesh)
	{
		CommandBuffers&  cmdBuffer = (*m_cmdBuffers.get());
		cmdBuffer.BeginRecording(cmdBufferIndex);
		
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearValues[1].depthStencil = { 0.0f, 1 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_renderPass->Handle();
		renderPassInfo.framebuffer = m_framebuffers[imageIndex]->Handle();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_SwapChain->GetExtent();
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(cmdBuffer[cmdBufferIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(cmdBuffer[cmdBufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline->Handle());

		VkBuffer vertexBuffers[] = { mesh.GetVertexbuffer().Handle()};
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(cmdBuffer[cmdBufferIndex], 0, 1, vertexBuffers, offsets);

		VkBuffer indexBuffer = mesh.GetIndexbuffer().Handle();
		VkDeviceSize offset = 0;
		vkCmdBindIndexBuffer(cmdBuffer[cmdBufferIndex], indexBuffer, offset, VK_INDEX_TYPE_UINT32);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_SwapChain->GetExtent().width);
		viewport.height = static_cast<float>(m_SwapChain->GetExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(cmdBuffer[cmdBufferIndex], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_SwapChain->GetExtent();
		vkCmdSetScissor(cmdBuffer[cmdBufferIndex], 0, 1, &scissor);

	 // test 
		std::vector<VkDescriptorSet>DescSets;
		DescSets.push_back(m_globalDescSets[cmdBufferIndex]->Handle());
		vkCmdBindDescriptorSets(cmdBuffer[cmdBufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout->Handle(), 0, 1, DescSets.data(), 0, nullptr);
		
		
		// Cherno: vkCmdDrawIndexed(commandBuffer, submesh.IndexCount, instanceCount, submesh.BaseIndex, submesh.BaseVertex, 0);
		// Picolo : m_vk_cmd_draw_indexed(m_vulkan_rhi->m_current_command_buffer,mesh->mesh_index_count,current_instance_count,0,0,	0);
		// 
		vkCmdDrawIndexed(cmdBuffer[cmdBufferIndex], mesh.GetIndexCount(), mesh.GetInstanceCount(), 0, 0, 0);
		vkCmdEndRenderPass(cmdBuffer[cmdBufferIndex]);

		cmdBuffer.EndRecording(cmdBufferIndex);
	}

	void VulkanRenderer::DrawFrame(const const Alalba::Mesh& mesh)
	{
		VkFence inflightFence = m_inFlightFences[m_currentFrame]->Handle();
		vkWaitForFences(m_device.Handle(), 1, &inflightFence, VK_TRUE, UINT64_MAX);
		

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(m_device.Handle(), m_SwapChain->Handle(), UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame]->Handle(), VK_NULL_HANDLE, &imageIndex);
		
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapChainAndFramebuffers();
			return;
		}
		else if (result == VK_SUBOPTIMAL_KHR)
		{
			RecreateSwapChainAndFramebuffers();
			return;
		}
		else {
			ALALBA_ASSERT(result == VK_SUCCESS, "Acquire Next Image Failed");
		}

		// update camera
///////////////////////////////////////////////////////////
		updateUniformBuffer(m_currentFrame);
//////////////////////////////////////////////////////////////
		vkResetFences(m_device.Handle(), 1, &inflightFence);
		vkResetCommandBuffer((*m_cmdBuffers.get())[m_currentFrame], 0);
		EncodeCommand(m_currentFrame, imageIndex,mesh);
		

		// submit
		VkCommandBuffer commandBuffers[]{ (*m_cmdBuffers.get())[m_currentFrame] };
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
		err = vkQueueSubmit(m_device.GetGraphicsQ().Handle(),
			1, &submitInfo, inflightFence);
		ALALBA_ASSERT(err == VK_SUCCESS, "Q submit failed");

		// present
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_SwapChain->Handle() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		result = vkQueuePresentKHR(m_device.GetGraphicsQ().Handle(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			RecreateSwapChainAndFramebuffers();
		}
		else {
			ALALBA_ASSERT(result == VK_SUCCESS, "Queue Present Failed");
		}
		m_currentFrame = (m_currentFrame + 1) % m_SwapChain->GetImgCount();
	}

}