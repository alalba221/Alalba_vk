#include "pch.h"
#include "BasicRenderSys.h"
#include "Alalba_VK/Vulkan/DescriptorSetLayout.h"
#include "Alalba_VK/Vulkan/PipelineCache.h"
#include "Alalba_VK/Core/Application.h"
#include "Alalba_VK/Vulkan/CommandBuffers.h"
#include "Alalba_VK/Core/Scene/Entity.h"
#include "Alalba_VK/Vulkan/RenderPass.h"

namespace Alalba 
{
	BasicRenderSys::BasicRenderSys(Scene& scene,
		const vk::RenderPass& renderpass, const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts, const vk::PipelineCache& pipelineCache)
	{
		CreateShaders("shaders/basic.vert.spv", "shaders/basic.frag.spv");
		CreatePipelineLayout(descriptorSetLayouts);
		CreatePipeline(renderpass, pipelineCache);
		PrepareDescriptorSets(scene, descriptorSetLayouts);
	}

	void BasicRenderSys::Render(Scene& scene, vk::CommandBuffers& cmdBuffers,
		const vk::DescriptorSet& globalDescSet, const int currentCmdBuffer)
	{
		// bind pipeline
		vkCmdBindPipeline(cmdBuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline->Handle());
		// bind global descriptor set
		VkDescriptorSet gbset = globalDescSet.Handle();
		vkCmdBindDescriptorSets(cmdBuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout->Handle(), 0, 1, &gbset, 0, nullptr);

		auto view = scene.GetAllEntitiesWith<MeshComponent, DescrpotrSetCompont>();
		
		/// for each entity
		for (auto entity : view)
		{
			auto [mesh, descriptorset] = view.get<MeshComponent, DescrpotrSetCompont>(entity);
			std::shared_ptr<Mesh> pmesh= mesh.m_Mesh;
			// 1. Get vertex and index buffer
			VkBuffer vertexBuffers[] = { pmesh->GetVertexbuffer().Handle() };
			VkBuffer IndexBuffers = pmesh->GetIndexbuffer().Handle();
			
			/// for currend command buffer, record command 
			{
				// TODO: 2. PUSHCONSTANT for model matrix
				// 3. bind vectex and index buffer to pipeline
				VkDeviceSize offset = 0;
				vkCmdBindVertexBuffers(cmdBuffers[currentCmdBuffer], 0, 1, vertexBuffers, &offset);
				vkCmdBindIndexBuffer(cmdBuffers[currentCmdBuffer], IndexBuffers, offset, VK_INDEX_TYPE_UINT32);

				// 4. bind local descriptors to pipeline 
				// start from index 1
				VkDescriptorSet modset = descriptorset.DescriptorSet->Handle();
				vkCmdBindDescriptorSets(cmdBuffers[currentCmdBuffer], VK_PIPELINE_BIND_POINT_GRAPHICS,
					m_pipelineLayout->Handle(), 1, 1, &modset, 0, nullptr);

				// 5. draw
				vkCmdDrawIndexed(cmdBuffers[currentCmdBuffer], pmesh->GetIndexCount(), pmesh->GetInstanceCount(), 0, 0, 0);
			}
		}
	}

	// private
	void BasicRenderSys::CreateShaders(const std::string& vertex, const std::string& frag)
	{
		const vk::Device& device = Application::Get().GetDevice();

		// create shader
		m_vertexShader = vk::ShaderModule::Builder(device)
			// path relative to Sandbox
			.SetTag("basic vertex Shader")
			.SelectSpvFile(vertex)
			.SetShaderStageBits(VK_SHADER_STAGE_VERTEX_BIT)
			.Build();

		m_fragShader = vk::ShaderModule::Builder(device)
			// path relative to Sandbox
			.SetTag("basic fragShader")
			.SelectSpvFile(frag)
			.SetShaderStageBits(VK_SHADER_STAGE_FRAGMENT_BIT)
			.Build();
	}

	void BasicRenderSys::CreatePipelineLayout(const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts)
	{
		const vk::Device& device = Application::Get().GetDevice();
		m_pipelineLayout = vk::PipelineLayout::Builder(device)
			.SetTag("baisc pipeline layout")
			.AddPushConstant(sizeof(glm::mat4))
			.BindDescriptorSetLayout(*descriptorSetLayouts[0])
			.BindDescriptorSetLayout(*descriptorSetLayouts[1])
			.Build()
			;
	}

	void BasicRenderSys::CreatePipeline(const vk::RenderPass& renderpass, const vk::PipelineCache& pipelineCache)
	{
		const vk::Device& device = Application::Get().GetDevice();

		m_graphicsPipeline = vk::GraphicsPipeline::Builder(device, *m_pipelineLayout, renderpass,
			pipelineCache)
			.SetTag("OBJ graphics pipeline")
			.AddPipelineStage(*m_vertexShader).AddPipelineStage(*m_fragShader)
			.SetVertexProcessingState(true)
			.SetTessellationState()
			.SetRasterizationState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE)
			.SetDepthState(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS)
			.SetMultisampleState(VK_SAMPLE_COUNT_1_BIT)
			.AddColorBlendAttachmentState(VK_COLOR_COMPONENT_R_BIT| VK_COLOR_COMPONENT_G_BIT| VK_COLOR_COMPONENT_B_BIT| VK_COLOR_COMPONENT_A_BIT, VK_FALSE)
			.SetColorBlendState(renderpass.ColorAttachmentCount())
			.SetViewportState(1,1)
			.SetDynamicState(VK_FALSE)
			.Build();
	}
	void BasicRenderSys::PrepareDescriptorSets(Scene& scene, const std::vector<const vk::DescriptorSetLayout*>& descriptorSetLayouts)
	{
		const vk::Device& device = Application::Get().GetDevice();

		m_descPool = vk::DescriptorPool::Builder(device)
			.SetTag("Basic Descriptor Pool")
			.SetMaxSets(1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
			.Build();

		m_allocator.reset(new vk::Allocator(device, Application::Get().GetVulkanInstance(), "Basic sys Allocator"));

		/// convert texture and translate component into DescrpotrSetCompont
		auto entities = scene.GetAllEntitiesWith<TextureComponent, TransformComponent>();
		for (auto e : entities)
		{
			Entity entity{ e,&scene };
			auto& entity_tag = entity.GetComponent<TagComponent>();

			// Add descriptor set to each entity
			std::string tag = entity_tag.Tag ;
	
			// use the second descriptor set ;descriptorSetLayouts[1]
			// DescSet for each model
			m_modelDescSets.insert(
				std::make_pair(
					tag,
					std::make_shared<vk::DescriptorSet>(device, *m_descPool, descriptorSetLayouts[1], tag + " descriptor set")
				)
			);
			
			//0. uniform buffer: convert glm::mat4 to uniformbuffer
			m_modelUBOs.insert(
				std::make_pair(
					tag,
					std::make_shared<vk::Buffer>(device, *m_allocator, sizeof(ModelUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, tag+ "'s uniform buffer")
				)
			);

			//1. texture
			std::shared_ptr<Texture> ptexture = entity.GetComponent<TextureComponent>().m_Texture;
			
			m_modelDescSets[tag]->BindDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
				*m_modelUBOs[tag],0,sizeof(ModelUBO));
			m_modelDescSets[tag]->BindDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
					ptexture->GetSampler(), ptexture->GetImageView(), ptexture->GetImage().Layout());
			m_modelDescSets[tag]->UpdateDescriptors();

			entity.AddComponent<DescrpotrSetCompont>(m_modelDescSets[tag]);
		}
	}
	void BasicRenderSys::Update(Scene& scene)
	{
		// update ubo
		auto entities = scene.GetAllEntitiesWith<DescrpotrSetCompont>();
		for (auto e : entities)
		{
			Entity entity = { e, &scene };
			std::string tag = entity.GetComponent<TagComponent>().Tag;
			glm::mat4& transform = entity.GetComponent<TransformComponent>().Transform;
			void* data = m_modelUBOs[tag]->MapMemory();
			memcpy(data, &transform, sizeof(transform));
			m_modelUBOs[tag]->UnMapMemory();
		}
		
	}
	void BasicRenderSys::ShutDown()
	{
		LOG_ERROR("shut down basic system");
		m_vertexShader->Clean();
		m_fragShader->Clean();
		m_pipelineLayout->Clean();
		m_graphicsPipeline->Clean();
		m_descPool->Clean();

		m_modelUBOs.clear();
		m_allocator->Clean();
	}

}
