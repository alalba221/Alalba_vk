#include "pch.h"
#include "ShadowMappingSys.h"
#include "Alalba_VK/Core/Application.h"
#include "Alalba_VK/Core/Scene/Entity.h"
#include "glm/glm.hpp"
#include <glm/gtx/quaternion.hpp>
namespace Alalba
{
	ShadowMappingSys::ShadowMappingSys(Scene& scene, const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts)
		:m_scene(scene)
	{
		Application& app = Application::Get();
		const vk::Device& device = app.GetDevice();

		/// 0. allocator and command pool for system
		m_allocator.reset(new vk::Allocator(device, app.GetVulkanInstance(), "ShadowMapSys Allocator"));

		m_cmdPool = vk::CommandPool::Builder(device)
			.SetTag("ShadowMapSys CmdPool")
			.SetFlags(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			//.SetFlags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			.SetQFamily(device.GetGraphicsQ(0).GetFamily())
			.Build();

		// 1. prepare frame buffer and renderpass
		PrepareOffscreenFramebuffer();
		// 2. Prepare and initialize uniform buffer containing shader uniforms
		PrepareUniformBuffer();
		// 3. descriptor set layout
		SetupDescriptorSetLayout(descriptorSetLayouts);

		// 4. pipeline
		CreateShaders("shaders/shadowmapp_offscreen.vert.spv", "shaders/shadowmapp_offscreen.frag.spv");
		m_pipelineCache = vk::PipelineCache::Builder(device)
			.SetTag("ShadowMapSys PipelineCache")
			.Build();
		PreparePipeline();

		// 5. Descriptor pool
		m_descPool = vk::DescriptorPool::Builder(device)
			.SetTag("ShadowMapSys Descriptor Pool")
			.SetMaxSets(vk::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, vk::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, vk::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.Build();

		// 6. DescriptorSet
		PrepareOffScreenDescripterSets();
	}
	void ShadowMappingSys::Update(Scene& scene, uint32_t currentFrame)
	{
		auto viewlight = scene.GetAllEntitiesWith<PointLightComponent>();
		for (auto e : viewlight)
		{
			Entity entity = { e, &scene };

			auto& position = entity.GetComponent<PointLightComponent>().LightPosition;
			auto& color = entity.GetComponent<PointLightComponent>().LightColor;
			
			m_ubo.zFar = zFar;
			m_ubo.zNear = zNear;
			m_ubo.lightProject = glm::perspective(glm::radians(45.0f), 1.0f, zNear, zFar);
			m_ubo.lightProject[1][1] *= -1;
			m_ubo.lightView = glm::lookAt(glm::vec3(position), glm::vec3(0.0f), glm::vec3(0, 1, 0));
			
			memcpy(m_shadowMapUBOs[currentFrame]->Mapped(), &m_ubo, sizeof(ShadowMappUbo));
			m_shadowMapUBOs[currentFrame]->Flush();
		}
	}
	void ShadowMappingSys::ShutDown()
	{
		LOG_ERROR("shut down shadow map system");
		
		m_vertShader->Clean();
		m_pipelineCache->Clean();
		m_pipelineLayout->Clean();
		m_pipeline->Clean();

		m_renderPass->Clean();

		for (auto& framebuffer : m_framebuffers)
			framebuffer->Clean();
		for (auto& depthImage : m_depthImages)
			depthImage->Clean();
		for (auto& depthImageView : m_depthImageViews)
			depthImageView->Clean();
		for (auto& sampler : m_samplers)
			sampler->Clean();
		for (auto& shadowMapUBO : m_shadowMapUBOs)
			shadowMapUBO->Clean();

		m_descPool->Clean();
		m_descSetLayout->Clean();
		for (auto& descriptorSet : m_descriptorSets)
			descriptorSet->Clean();

		m_cmdPool->Clean();
		m_allocator->Clean();
	}
	void ShadowMappingSys::PrepareOffScreenRenderPass()
	{
		Application& app = Application::Get();
		const vk::Device& device = app.GetDevice();


		/// Render Pass
		vk::RenderPass::Builder renderPassBuilder(device);
		renderPassBuilder.AddAttachment(m_depthImages[0]->GetFormat(), VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);

		vk::SubPass subPass0{};
		subPass0.UseAttachment(0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, vk::AttachmentType::Depth)
			.SetDescription(false);

		// 2 subpass denpendency for color and depth
		//  same as https://github.com/SaschaWillems/Vulkan/blob/master/examples/subpasses/subpasses.cpp
		// or 1 subpass dependency for both using or operation like the https://vulkan-tutorial.com/Depth_buffering
		renderPassBuilder.AddSubPass(subPass0)
			.SetSubPassDependencies(VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT,
						0, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)
			.SetSubPassDependencies(0, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
						VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT);

		m_renderPass = renderPassBuilder.Build();

	}
	void ShadowMappingSys::PrepareOffscreenFramebuffer()
	{
		Application& app = Application::Get();
		const vk::Device& device = app.GetDevice();

		for (int i = 0; i < vk::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_depthImages[i] = vk::Image::Builder(device, *m_allocator)
				.SetTag("ShadowMapSys DepthImage")
				.SetImgType(VK_IMAGE_TYPE_2D)
				.SetImageFormat(device.FindSupportedFormat(
					{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
					VK_IMAGE_TILING_OPTIMAL,
					VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
				))// this should be compatible with framebuffer
				.SetImageTiling(VK_IMAGE_TILING_OPTIMAL)
				.SetUsageFlags(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
				.SetImgExtent(VkExtent3D{ SHADOWMAP_DIM, SHADOWMAP_DIM,1 })
				.Build();
			m_depthImages[i]->TransitionImageLayout(*m_cmdPool, device.GetGraphicsQ(0), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

			m_depthImageViews[i] = vk::ImageView::Builder(device, *m_depthImages[i])
				.SetTag("ShadowMapSys DepthImageView")
				.SetViewType(VK_IMAGE_VIEW_TYPE_2D)
				.SetSubresourceAspectFlags(VK_IMAGE_ASPECT_DEPTH_BIT)
				.SetFormat(m_depthImages[i]->GetFormat())
				.Build();

			m_samplers[i] = vk::Sampler::Builder(device)
				.SetTag("ShadowMapSys DepthImage Sampler")
				.SetFilter(device.FormatIsFilterable(m_depthImages[i]->GetFormat(), m_depthImages[i]->GetTiling()) ? VK_FILTER_LINEAR : VK_FILTER_NEAREST)
				.SetMipmapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR)
				.SetAddressMode(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
				.Build();
		}
		
		PrepareOffScreenRenderPass();

		for (int i = 0; i < vk::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_framebuffers[i] = vk::FrameBuffer::Builder(device, *m_renderPass)
				.SetTag("ShadowMapSys Framebuffer")
				.SetWidthHeight(SHADOWMAP_DIM, SHADOWMAP_DIM)
				.PushAttachment(*m_depthImageViews[i])
				.Build();
		}
	}
	void ShadowMappingSys::PrepareUniformBuffer()
	{
		Application& app = Application::Get();
		const vk::Device& device = app.GetDevice();
		for (int i = 0; i < vk::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_shadowMapUBOs[i] = vk::Buffer::Builder(device, *m_allocator)
				.SetTag("ShadowMapSys Uniform Buffer")
				.SetSize(sizeof(ShadowMappUbo))
				.SetUsage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
				.SetVmaUsage(VMA_MEMORY_USAGE_CPU_TO_GPU)
				.Build();

			m_shadowMapUBOs[i]->MapMemory();
		}
	}
	void ShadowMappingSys::SetupDescriptorSetLayout(const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts)
	{
		const vk::Device& device = Application::Get().GetDevice();

		m_descSetLayout = vk::DescriptorSetLayout::Builder(device)
			// Binding 0 : Vertex shader uniform buffer
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			//// Binding 1 : Fragment shader image sampler (shadow map)
			//.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.SetTag("ShadowMapSys Descriptor Set Layout")
			.Build();

		m_pipelineLayout = vk::PipelineLayout::Builder(device)
			.SetTag("ShadowMapSys pipeline layout")
			.AddPushConstant(sizeof(glm::mat4))
			.BindDescriptorSetLayout(*m_descSetLayout)
			.BindDescriptorSetLayout(*descriptorSetLayouts[1])
			.Build();
	}
	void ShadowMappingSys::CreateShaders(const std::string& vertexShader, const std::string& fragShader)
	{
		const vk::Device& device = Application::Get().GetDevice();
		m_vertShader = vk::ShaderModule::Builder(device)
			// path relative to Sandbox
			.SetTag("ShadowMapSys vertex Shader")
			.SelectSpvFile(vertexShader)
			.SetShaderStageBits(VK_SHADER_STAGE_VERTEX_BIT)
			.Build();

		//m_offScreenFragShader = vk::ShaderModule::Builder(device)
		//	// path relative to Sandbox
		//	.SetTag("ShadowMapSys fragment Shader")
		//	.SelectSpvFile(fragShader)
		//	.SetShaderStageBits(VK_SHADER_STAGE_FRAGMENT_BIT)
		//	.Build();
	}

	void ShadowMappingSys::PreparePipeline()
	{
		const vk::Device& device = Application::Get().GetDevice();

		m_pipeline = vk::GraphicsPipeline::Builder(device, *m_pipelineLayout, *m_renderPass, *m_pipelineCache)
			.SetTag("ShadowMapSys graphics pipeline")
			.AddPipelineStage(*m_vertShader)//.AddPipelineStage(*m_offScreenFragShader)
			.SetVertexProcessingState(true)
			.SetTessellationState()
			.SetRasterizationState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_TRUE)
			.SetDepthState(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL)
			.SetMultisampleState(VK_SAMPLE_COUNT_1_BIT)
			//.AddColorBlendAttachmentState(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, VK_FALSE)
			//.SetColorBlendState(m_renderPass->ColorAttachmentCount()/*0*/)
			.SetViewportState(1, 1)
			.SetDynamicState(true)
			.Build();
	}

	void ShadowMappingSys::PrepareOffScreenDescripterSets()
	{
		const vk::Device& device = Application::Get().GetDevice();
		for (int i = 0; i < vk::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_descriptorSets[i] = vk::DescriptorSet::Allocator(device, *m_descPool)
				.SetTag("ShadowMapSys OffScreen DescriptorSet")
				.SetDescSetLayout(*m_descSetLayout)
				.Allocate();
			m_descriptorSets[i]->
				BindDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, *m_shadowMapUBOs[i], 0, sizeof(ShadowMappUbo))
				.UpdateDescriptors();
		}
	}

	void ShadowMappingSys::GenerateShadowMapp(vk::CommandBuffers& cmdBuffers, const uint32_t currentCmdBuffer)
	{
		VkClearValue clearValue;
		clearValue.depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_renderPass->Handle();
		renderPassInfo.framebuffer = m_framebuffers[currentCmdBuffer]->Handle();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent.width = SHADOWMAP_DIM;
		renderPassInfo.renderArea.extent.height = SHADOWMAP_DIM;
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearValue;

		vkCmdBeginRenderPass(cmdBuffers[currentCmdBuffer], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(SHADOWMAP_DIM);
		viewport.height = static_cast<float>(SHADOWMAP_DIM);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent.width = SHADOWMAP_DIM;
		scissor.extent.height = SHADOWMAP_DIM;
		
		vkCmdSetViewport(cmdBuffers[currentCmdBuffer], 0, 1, &viewport);

	
		vkCmdSetScissor(cmdBuffers[currentCmdBuffer], 0, 1, &scissor);

		// Set depth bias (aka "Polygon offset")
		// Required to avoid shadow mapping artifacts
		vkCmdSetDepthBias(
			cmdBuffers[currentCmdBuffer],
			m_depthBiasConstant,
			0.0f,
			m_depthBiasSlope);

		vkCmdBindPipeline(cmdBuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->Handle());
		VkDescriptorSet offScreenDescSet = m_descriptorSets[currentCmdBuffer]->Handle();
		vkCmdBindDescriptorSets(cmdBuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout->Handle(), 0, 1, &offScreenDescSet, 0, nullptr);
		
		auto view = m_scene.GetAllEntitiesWith<GLTFComponent, TransformComponent>();

		/// for each entity
		for (auto entity : view)
		{

			auto model = view.get<GLTFComponent>(entity).Model;
			auto basetransform = view.get<TransformComponent>(entity).Transform;

			//DrawModel(*model, basetransform, cmdBuffers, currentCmdBuffer);
			model->DrawModel(basetransform, *m_pipeline, cmdBuffers, currentCmdBuffer);
		}

		vkCmdEndRenderPass(cmdBuffers[currentCmdBuffer]);

	}


	/// Debug

}