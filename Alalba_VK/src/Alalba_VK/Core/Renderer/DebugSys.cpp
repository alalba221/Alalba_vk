#include "pch.h"
#include "DebugSys.h"


#include "Alalba_VK/Core/Application.h"
#include "Alalba_VK/Vulkan/CommandBuffers.h"
#include "Alalba_VK/Core/Scene/Entity.h"
#include "Alalba_VK/Vulkan/RenderPass.h"
#include "Alalba_VK/Vulkan/FrameBuffer.h"
namespace Alalba
{
	DebugSys::DebugSys(const vk::RenderPass& renderpass, 
		VkImageLayout imageLayout, const vk::ImageView& imageview, const vk::Sampler& sampler)
	{
		Application& app = Application::Get();
		const vk::Device& device = app.GetDevice();

		/// 0. allocator and command pool for system
		m_allocator.reset(new vk::Allocator(device, app.GetVulkanInstance(), "Debug Allocator"));
		
		// 1. prepare frame buffer and renderpass
		// no need 
		
		// 2. Prepare and initialize uniform buffer containing shader uniforms
		PrepareUniformBuffer();
		// 3. descriptor set layout
		SetupDescriptorSetLayout();

		// 4. pipeline
		CreateShaders("shaders/quad.vert.spv", "shaders/quad.frag.spv");
		m_pipelineCache = vk::PipelineCache::Builder(device)
			.SetTag("DebugSys PipelineCache")
			.Build();
		PreparePipeline(renderpass);
		// 5. Descriptor pool
		m_descPool = vk::DescriptorPool::Builder(device)
			.SetTag("DebugSys Descriptor Pool")
			.SetMaxSets(3)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3)
			.Build();
		// 6. DescriptorSet
		PrepareDescripterSet(imageLayout, imageview, sampler);
		
		
		
	}

	//void DebugSys::Render( vk::CommandBuffers& cmdBuffers,const int currentCmdBuffer)
	//{
	//	
	//}

	// private
	void DebugSys::CreateShaders(const std::string& vertex, const std::string& frag)
	{
		const vk::Device& device = Application::Get().GetDevice();

		// create shader
		m_vertexShader = vk::ShaderModule::Builder(device)
			// path relative to Sandbox
			.SetTag("DebugSys vertex Shader")
			.SelectSpvFile(vertex)
			.SetShaderStageBits(VK_SHADER_STAGE_VERTEX_BIT)
			.Build();

		m_fragShader = vk::ShaderModule::Builder(device)
			// path relative to Sandbox
			.SetTag("DebugSys fragShader")
			.SelectSpvFile(frag)
			.SetShaderStageBits(VK_SHADER_STAGE_FRAGMENT_BIT)
			.Build();
	}

	void DebugSys::PrepareUniformBuffer()
	{
		Application& app = Application::Get();
		const vk::Device& device = app.GetDevice();

		m_debugUBO = vk::Buffer::Builder(device, *m_allocator)
			.SetTag("DebugSys Uniform Buffer")
			.SetSize(sizeof(DebugUbo))
			.SetUsage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
			.SetVmaUsage(VMA_MEMORY_USAGE_CPU_TO_GPU)
			.Build();

		m_debugUBO->MapMemory();
	}

	void DebugSys::SetupDescriptorSetLayout()
	{
		const vk::Device& device = Application::Get().GetDevice();

		m_descSetLayout = vk::DescriptorSetLayout::Builder(device)
			// Binding 0 : Vertex shader uniform buffer
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
			// Binding 1 : Fragment shader image sampler (shadow map)
			.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.SetTag("DebugSys Descriptor Set Layout")
			.Build();

		m_pipelineLayout = vk::PipelineLayout::Builder(device)
			.SetTag("DebugSys pipeline layout")
			// .AddPushConstant(sizeof(glm::mat4))
			.BindDescriptorSetLayout(*m_descSetLayout)
			.Build();
	}

	void DebugSys::PreparePipeline(const vk::RenderPass& renderpass)
	{
		const vk::Device& device = Application::Get().GetDevice();

		m_graphicsPipeline = vk::GraphicsPipeline::Builder(device, *m_pipelineLayout, renderpass, *m_pipelineCache)
			.SetTag("DebugSys  pipeline")
			.AddPipelineStage(*m_vertexShader).AddPipelineStage(*m_fragShader)
			.SetVertexProcessingState(false)
			.SetTessellationState()
			.SetRasterizationState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE)
			.SetDepthState(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL)
			.SetMultisampleState(VK_SAMPLE_COUNT_1_BIT)
			.AddColorBlendAttachmentState(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, VK_FALSE)
			.SetColorBlendState(1)
			.SetViewportState(1, 1)
			.SetDynamicState(false)
			.Build();
	}

	void DebugSys::BuildCommandBuffer(const vk::RenderPass& renderpass, const vk::FrameBuffer& framebuffer, VkExtent2D areaExtend,
		const vk::CommandBuffers& cmdBuffers, const uint32_t currentCmdBuffer)
	{
		VkClearValue clearValues[2];
		clearValues[0].color = { { 0.025f, 0.025f, 0.025f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		renderPassBeginInfo.renderPass = renderpass.Handle();
		renderPassBeginInfo.framebuffer = framebuffer.Handle();
		renderPassBeginInfo.renderArea.extent = areaExtend;
		renderPassBeginInfo.renderArea.offset = { 0,0 };
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(cmdBuffers[currentCmdBuffer], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)areaExtend.width;
		viewport.height = (float)areaExtend.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = areaExtend;
		vkCmdSetViewport(cmdBuffers[currentCmdBuffer], 0, 1, &viewport);
		vkCmdSetScissor(cmdBuffers[currentCmdBuffer], 0, 1, &scissor);
		// Visualize shadow map
		VkDescriptorSet DebugdescSet = m_descSet->Handle();
		vkCmdBindDescriptorSets(cmdBuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout->Handle(), 0, 1, &DebugdescSet, 0, nullptr);
		vkCmdBindPipeline(cmdBuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline->Handle());
		vkCmdDraw(cmdBuffers[currentCmdBuffer], 3, 1, 0, 0);
	

		vkCmdEndRenderPass(cmdBuffers[currentCmdBuffer]);
	}

	void DebugSys::PrepareDescripterSet(VkImageLayout imageLayout, const vk::ImageView& imageview, const vk::Sampler& sampler)
	{
		const vk::Device& device = Application::Get().GetDevice();
		m_descSet = vk::DescriptorSet::Allocator(device, *m_descPool)
			.SetTag("DebugSysDescriptorSet")
			.SetDescSetLayout(*m_descSetLayout)
			.Allocate();
		m_descSet->
			BindDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, *m_debugUBO, 0, sizeof(DebugUbo))
			.BindDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, sampler, imageview, imageLayout)
			.UpdateDescriptors();
	}


	void DebugSys::Update()
	{
		memcpy(m_debugUBO->Mapped(), &m_ubo, sizeof(DebugUbo));
	}

	void DebugSys::ShutDown()
	{
		LOG_ERROR("shut down Debug system");
		m_vertexShader->Clean();
		m_fragShader->Clean();
		m_pipelineCache->Clean();

		m_pipelineLayout->Clean();
		m_graphicsPipeline->Clean();

		m_descSetLayout->Clean();

		m_descPool->Clean();
		m_descSet->Clean();
		m_debugUBO->Clean();
		m_allocator->Clean();
	}
}
