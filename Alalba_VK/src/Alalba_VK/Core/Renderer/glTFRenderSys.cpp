#include "pch.h"
#include "glTFRenderSys.h"
#include "Alalba_VK/Vulkan/DescriptorSetLayout.h"
#include "Alalba_VK/Vulkan/PipelineCache.h"
#include "Alalba_VK/Core/Application.h"
#include "Alalba_VK/Vulkan/CommandBuffers.h"
#include "Alalba_VK/Core/Scene/Entity.h"
#include "Alalba_VK/Vulkan/RenderPass.h"
namespace Alalba
{
	glTFRenderSys::glTFRenderSys(Scene& scene,
		const vk::RenderPass& renderpass, const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts, const vk::PipelineCache& pipelineCache)
		:m_scene(scene)
	{
		CreateShaders("shaders/gltf.vert.spv", "shaders/gltf.frag.spv");
		CreatePipelineLayout(descriptorSetLayouts);
		CreatePipeline(renderpass, pipelineCache);

		m_UI.reset(new UIOverlay(renderpass));
	}

	void glTFRenderSys::BuildCommandBuffer(const vk::RenderPass& renderpass, const vk::FrameBuffer& framebuffer, VkExtent2D areaExtend, 
		const vk::DescriptorSet& globalDescSet,
		const vk::CommandBuffers& cmdBuffers, const uint32_t currentCmdBuffer)
	{
		VkClearValue clearValues[2];
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderpass.Handle();
		renderPassInfo.framebuffer = framebuffer.Handle();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = areaExtend;
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues;
	
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = areaExtend.width;
		viewport.height = areaExtend.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = areaExtend;

		///***************/
		//m_UI->NewFrame();
		//m_UI->UpdateBuffers();
		///***************/

		vkCmdBeginRenderPass(cmdBuffers[currentCmdBuffer], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdSetViewport(cmdBuffers[currentCmdBuffer], 0, 1, &viewport);
		vkCmdSetScissor(cmdBuffers[currentCmdBuffer], 0, 1, &scissor);

		vkCmdBindPipeline(cmdBuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline->Handle());
		// bind global descriptor set
		VkDescriptorSet gbset = globalDescSet.Handle();
		vkCmdBindDescriptorSets(cmdBuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout->Handle(), 0, 1, &gbset, 0, nullptr);

		auto view = m_scene.GetAllEntitiesWith<GLTFComponent, TransformComponent>();

		/// for each entity
		for (auto entity : view)
		{

			auto model = view.get<GLTFComponent>(entity).Model;
			auto basetransform = view.get<TransformComponent>(entity).Transform;

			//DrawModel(*model, basetransform, cmdBuffers, currentCmdBuffer);
			model->DrawModel(basetransform, *m_graphicsPipeline, cmdBuffers, currentCmdBuffer);
		}

		m_UI->Draw(cmdBuffers, currentCmdBuffer);

		vkCmdEndRenderPass(cmdBuffers[currentCmdBuffer]);
	}

	// private
	void glTFRenderSys::CreateShaders(const std::string& vertex, const std::string& frag)
	{
		const vk::Device& device = Application::Get().GetDevice();

		// create shader
		m_vertexShader = vk::ShaderModule::Builder(device)
			// path relative to Sandbox
			.SetTag("glTF vertex Shader")
			.SelectSpvFile(vertex)
			.SetShaderStageBits(VK_SHADER_STAGE_VERTEX_BIT)
			.Build();

		m_fragShader = vk::ShaderModule::Builder(device)
			// path relative to Sandbox
			.SetTag("glTF fragShader")
			.SelectSpvFile(frag)
			.SetShaderStageBits(VK_SHADER_STAGE_FRAGMENT_BIT)
			.Build();
	}

	void glTFRenderSys::CreatePipelineLayout(const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts)
	{
		const vk::Device& device = Application::Get().GetDevice();

		m_pipelineLayout = vk::PipelineLayout::Builder(device)
			.SetTag("gltf pipeline layout")
			.AddPushConstant(sizeof(glm::mat4))
			.BindDescriptorSetLayout(*descriptorSetLayouts[0])
			.BindDescriptorSetLayout(*descriptorSetLayouts[1])
			.Build()
			;

	}

	void glTFRenderSys::CreatePipeline(const vk::RenderPass& renderpass, const vk::PipelineCache& pipelineCache)
	{
		const vk::Device& device = Application::Get().GetDevice();

		m_graphicsPipeline = vk::GraphicsPipeline::Builder(device, *m_pipelineLayout, renderpass, pipelineCache)
			.SetTag("GLTF graphics pipeline")
			.AddPipelineStage(*m_vertexShader).AddPipelineStage(*m_fragShader)
			.SetVertexProcessingState(true)
			.SetTessellationState()
			.SetRasterizationState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE)
			.SetDepthState(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS)
			.SetMultisampleState(VK_SAMPLE_COUNT_1_BIT)
			.AddColorBlendAttachmentState(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, VK_FALSE)
			.SetColorBlendState(renderpass.ColorAttachmentCount())
			.SetViewportState(1, 1)
			.SetDynamicState(VK_FALSE)
			.Build();

	}

	void glTFRenderSys::Update()
	{
		// update UI
		//m_UI->NewFrame();
		//cmdNeedUpdate = m_UI->UpdateBuffers();
	}
	void glTFRenderSys::ShutDown()
	{
		ALALBA_ERROR("shut downgltf system");
		m_vertexShader->Clean();
		m_fragShader->Clean();
		m_pipelineLayout->Clean();
		m_graphicsPipeline->Clean();
	}
}
